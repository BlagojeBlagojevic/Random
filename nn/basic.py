import torch
import numpy as np
import matplotlib.pyplot as plt


class Inveres_Kinematics(torch.nn.Module):
  def __init__(self, nIn, nOut, nNeuron, nLayer) -> None:
    super(Inveres_Kinematics, self).__init__()
    #define model
    #self.r = torch.nn.ReLU();
    self.lIn = torch.nn.Linear(nIn, nNeuron, bias = True)
    self.r1 = torch.nn.Tanh()
    self.l1 = torch.nn.Linear(nNeuron, nNeuron, bias = True)
    self.r2 = torch.nn.Tanh()
    self.lOut = torch.nn.Linear(nNeuron, nOut, bias = True)

  def forward(self, input):
    out = self.lIn(input)
    out = self.r1(out)
    out = self.l1(out)
    out = self.r2(out)
    out = self.lOut(out)
    return out

def training(model, nIteration, error, optim, inputData, trainingData):
  loss = []
    #error = self.error;
  for epoch in range(nIteration):
    prediction = model(inputData)
      #LOSS=loss(CLOSED,PRED)
    err = error(prediction, trainingData)
    loss.append(err.item())
    model.zero_grad()
    err.backward()
    optim.step()
    if(epoch % 100 == 0):
      print("loss in ", epoch, "is ", err.item())
  plt.plot(loss)



model = Inveres_Kinematics(2, 2, 2, 2)
a = np.array([2, 2])
a = torch.tensor(a).float()

#optim = torch.optim.Adam(model.parameters(),lr=0.01)
#err = torch.nn.MSELoss()#torch.nn.MSELoss()
#training(model, 100, err, optim, a, a)

training(model, 100, torch.nn.MSELoss(), torch.optim.Adam(model.parameters(), lr = 0.01), a, a)
print(model.forward(a))

