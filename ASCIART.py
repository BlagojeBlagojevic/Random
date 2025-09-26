#!/usr/bin/env python3

import sys
import argparse
import shutil
from PIL import Image, ImageDraw, ImageFont, ImageEnhance
import numpy as np
import os


# Characters from light to dark
orig_chars = ' .,:;irsXA253hMHGS#9B&@'
chars = np.asarray(list(reversed(orig_chars)))


def image_to_ascii_png(input_path, output_path, args):
    """
    Converts a single image file to a PNG image of ASCII art.
    """
    try:
        img_pil = Image.open(input_path)
    except Exception as e:
        print(f"Error: Could not open or process {input_path}. Skipping. ({e})")
        return

    # --- Font Handling ---
    font_size = 10
    try:
        font_names = [
            "DejaVuSansMono.ttf",
            "Consolas.ttf",
            "Courier New.ttf",
            "cour.ttf",
        ]
        font = None
        for name in font_names:
            try:
                font = ImageFont.truetype(name, size=font_size)
                break
            except IOError:
                continue
        if font is None:
            print("Warning: Monospaced font not found. Falling back to default font. Alignment may be incorrect.")
            font = ImageFont.load_default()
    except Exception as e:
        print(f"Fatal: Could not load any font. Please install a monospaced font. Error: {e}")
        return

    bbox = font.getbbox("W")
    char_width = bbox[2] - bbox[0]
    char_height = bbox[3] - bbox[1]

    if char_width <= 0 or char_height <= 0:
        print("Warning: Could not determine font size. Using fallback values.")
        char_width, char_height = 6, 12

    termsize = shutil.get_terminal_size()

    if args.auto == "h":
        auto_factor = termsize[1] / img_pil.size[1]
    else:
        auto_factor = termsize[0] / (img_pil.size[0] * args.pixel)

    S = (
        round(img_pil.size[0] * args.scale * args.pixel * auto_factor),
        round(img_pil.size[1] * args.scale * auto_factor),
    )
    num_cols, num_rows = S

    # --- Create Image Canvas ---
    img_width_px = int(num_cols * char_width)
    img_height_px = int(num_rows * char_height)

    if args.transparent:
        output_image = Image.new("RGBA", (img_width_px, img_height_px), (0, 0, 0, 0))
    else:
        output_image = Image.new("RGB", (img_width_px, img_height_px), color=(255, 255, 255))

    draw = ImageDraw.Draw(output_image)

    # Convert source to RGB for consistency
    rgb_im = img_pil.convert("RGB").resize(S)
    rgb_arr = np.array(rgb_im)

    # Grayscale intensity for ASCII mapping
    img_gray = np.sum(np.asarray(img_pil.convert("RGB").resize(S)), axis=2)
    img_gray -= img_gray.min()
    if img_gray.max() != 0:
        img_gray = (1.0 - img_gray / img_gray.max()) ** args.ascii * (chars.size - 1)
    else:
        img_gray = np.zeros_like(img_gray)

    # --- Draw characters ---
    for y in range(num_rows):
        for x in range(num_cols):
            char_index = int(img_gray[y, x])
            char = chars[char_index]
            r, g, b = rgb_arr[y, x]
            pos_x = x * char_width
            pos_y = y * char_height
            if args.transparent:
                # get alpha from original if present
                rgba_pixel = img_pil.convert("RGBA").resize(S).getpixel((x, y))
                _, _, _, alpha = rgba_pixel

                if char == " " or alpha < 10:
                    # skip spaces and nearly transparent pixels
                    continue

                draw.text(
                    (pos_x, pos_y),
                    char,
                    fill=(r, g, b, alpha),  # use original alpha
                    font=font
                )
            else:
                draw.text((pos_x, pos_y), char, fill=(r, g, b), font=font)

          

            draw.text((pos_x, pos_y), char, fill=(r, g, b, 255) if args.transparent else (r, g, b), font=font)

    # --- Brightness Adjustment ---
    if args.brightness != 1.0:
        enhancer = ImageEnhance.Brightness(output_image)
        output_image = enhancer.enhance(args.brightness)

    try:
        output_image.save(output_path)
        print(f"Successfully converted {os.path.relpath(input_path)} -> {os.path.relpath(output_path)}")
    except Exception as e:
        print(f"Error: Could not save image {output_path}. ({e})")


def main():
    parser = argparse.ArgumentParser(
        description="Transforms all images in a folder and its subfolders into PNG images of ASCII art.",
        epilog="The input and output folders are mandatory.",
    )

    group_input = parser.add_argument_group("Basic arguments")
    group_input.add_argument("-i", "--input", required=True, help="Path to the root input FOLDER containing images.")
    group_input.add_argument("-o", "--output", required=True, help="Path to the root output FOLDER for ASCII art PNG files.")
    group_input.add_argument("-a", "--ascii", default=1.0, type=float, metavar="FLOAT", help="ASCII level correction factor.")
    group_input.add_argument(
        "-b", "--brightness", default=1.2, type=float, metavar="FLOAT",
        help="Adjust brightness of final image (1.0 = original, >1.0 = lighter, <1.0 = darker)."
    )
    group_input.add_argument(
        "-t", "--transparent", action="store_true",
        help="Make background transparent (spaces are left transparent)."
    )

    group_args = parser.add_argument_group("Output scale arguments")
    group_args.add_argument("--auto", default="h", choices=["h", "w"], help="Automatically scale output based on terminal height (h) or width (w).")
    group_args.add_argument("-s", "--scale", default=1.0, type=float, metavar="FLOAT", help="Rescale the output by this correction factor.")
    group_args.add_argument("-p", "--pixel", default=1.0, type=float, metavar="FLOAT", help="Character aspect ratio correction. Use 1.0 for square PNG pixels.")

    args = parser.parse_args()

    if args.pixel != 1.0:
        print("Note: For PNG output, a pixel aspect ratio (-p) of 1.0 is recommended for best results.")

    input_folder = args.input
    output_folder = args.output

    if not os.path.isdir(input_folder):
        print(f"Error: Input path '{input_folder}' is not a valid directory.")
        sys.exit(1)

    supported_extensions = (".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tiff")

    print(f"Scanning '{input_folder}' and all subfolders for images...")

    for dirpath, _, filenames in os.walk(input_folder):
        for filename in filenames:
            if filename.lower().endswith(supported_extensions):
                input_path = os.path.join(dirpath, filename)
                relative_path = os.path.relpath(dirpath, input_folder)

                if relative_path == ".":
                    output_dir = output_folder
                else:
                    output_dir = os.path.join(output_folder, relative_path)

                os.makedirs(output_dir, exist_ok=True)

                output_filename = os.path.splitext(filename)[0] + ".png"
                output_path = os.path.join(output_dir, output_filename)

                image_to_ascii_png(input_path, output_path, args)

    print("Processing complete.")


if __name__ == "__main__":
    main()
