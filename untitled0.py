# -*- coding: utf-8 -*-

import random
import numpy as np
import matplotlib.pyplot as plt 

class Value:
    
    def __init__(self,data, children = (), operation = ''):
        self.prev = set(children)
        self.grad = 0.0
        self.data = data
        self.operation = operation
        self.backward = lambda:None
    
    def __repr__(self): #Print
        return f"Value(data = {self.data})"        
    
    #main math operation
    def __add__(self,b):
        b = b if isinstance(b, Value) else Value(b)   #Convert to value
        out = Value(self.data + b.data,(self,b), '+')
        def backward():   #Represent Derivative
            self.grad += out.grad  
            b.grad    += out.grad
        out.backward = backward    
        return out    
    
    def __mul__(self,b):
        b = b if isinstance(b, Value) else Value(b)
        out = Value(self.data * b.data, (self, b), '*')
        def backward():
            self.grad += out.grad * b.data
            b.grad    += out.grad * self.data
        out.backward = backward
        return out
    
    
    def __pow__(self,b):  #self.data^b
        out = Value(self.data**b, (self,),  f'**{b}')
        def backward():
            self.grad += (b * self.data**(b-1)) * out.grad
        out.backward = backward        
        return out
    
    #const operation
    def __neg__(self): # -self
        return self * -1

    def __radd__(self, other): # other + self
        return self + other

    def __sub__(self, other): # self - other
        return self + (-other)

    def __rsub__(self, other): # other - self
        return other + (-self)

    def __rmul__(self, other): # other * self
        return self * other

    def __truediv__(self, other): # self / other
        return self * other**-1

    def __rtruediv__(self, other): # other / self
        return other * self**-1

    def __repr__(self):
        return f"Value(data={self.data}, grad={self.grad})"

    #Activation function    
    def relu(self):
        out = Value(0 if self.data < 0 else self.data, (self,), 'ReLU')
        def backward():
            self.grad += (out.data > 0) * out.grad
        out.backward = backward
        return out
    #Gradient calculation
    def gradient(self):
        # Topological graph 
        # https://www.geeksforgeeks.org/topological-sorting/
        
        topo = []
        visited = set()
        def build_topo(v):
            if v not in visited:
                visited.add(v)
                for child in v.prev:
                    build_topo(child)
                topo.append(v)
        build_topo(self)

        # go one variable at a time and apply 
        # the chain rule to get its gradient
        self.grad = 1
        for v in reversed(topo):
            v.backward()


class Module:
    
    def zero_grad(self):
        for i in self.parametars():
            i.grad = 0
    
    def parametars(self):
        return []

#Consist of list of weigts from previous laye +bias
class Neuron(Module):
    
    def __init__(self,nin,nonlin = True):
        self.weights = [Value(random.uniform(-1, 1)) for _ in range(nin)]    
        self.b       = Value(0)
        self.nonlin = nonlin

    #If like function call input 
    def __call__(self, x):
        act = sum((wi*xi for wi,xi in zip(self.weights, x)), self.b)
        return act.relu() if self.nonlin else act
        
    def parameters(self):
        return self.weights + [self.b]
        #print("Neuron")
    
    def __repr__(self):
        return f"{'ReLU' if self.nonlin else 'Linear'}Neuron({len(self.weights)})"
    

class Layer(Module):

    def __init__(self, nin, nout, **kwargs):
        self.neurons = [Neuron(nin, **kwargs) for _ in range(nout)]

    def __call__(self, x):
        out = [n(x) for n in self.neurons]
        return out[0] if len(out) == 1 else out

    def parameters(self):
        return [p for n in self.neurons for p in n.parameters()]
        print("Layer")
    def __repr__(self):
        return f"Layer of [{', '.join(str(n) for n in self.neurons)}]"
    
    

class MLP(Module):

    def __init__(self, nin, nouts):
        sz = [nin] + nouts
        self.layers = [Layer(sz[i], sz[i+1], nonlin=i!=len(nouts)-1) for i in range(len(nouts))]
       
    def __call__(self, x):
        for layer in self.layers:
            x = layer(x)
        return x

    def parameters(self):
        return [p for layer in self.layers for p in layer.parameters()]
        #print("Mlp")
    def __repr__(self):
        return f"MLP of [{', '.join(str(layer) for layer in self.layers)}]"
    
    
    

x = [
     [0,0],
     [1,0],
     [0,1],
     [1,1],     
     ]
y = [0,1,1,0]

model = MLP(2, [16, 1]) # 2-layer neural network
#print(model.layers[0].neurons[0].weights[0])
#print(model.parameters())
#print("number of parameters", len(model.parameters()))


yp = [model(i) for i in x] 

loss = sum((yout - yg)**2 for yg,yout in zip(y,yp)) 



#print(model.parametars())

grap = []
for iteration in range(400):
    
    yp = [model(i) for i in x] 
    loss = sum((yout - yg)**2 for yg,yout in zip(y,yp))
    if (loss.data <= 0.02):
        break
    grap.append(loss.data)
    model.zero_grad()
    loss.gradient()
    
    for p in model.parameters():
        p.data += -0.01 * p.grad
    
        
  
    print(f"Loss ({loss})") 

plt.plot(grap)

yp = [model(i) for i in x]
print("Outs \n",yp)