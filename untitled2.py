# Actiation function is used in linear layer
def activationFunction(x):
  return 1/(1+np.exp(-1*x))

#Softmax returns np vector of e^(x-max(x))/sum(e^(x-max(x))
def softmax(x):
    exp_x = np.exp(x)  # exp_x = np.exp(x-np.max(x))
    return exp_x / np.sum(exp_x)

#x = np.array([1,1,0])
#Layer normalisation shoud be used for limiting value of layers for vanising gradient isl
#average of 0 variance of 1
def layerNormalisaition(x,gama,beta):
  #np.var = meen(sum(abs(a - a.mean())^2)) displacment from gausian distribution
  var = np.var(x)
  mean = np.mean(x)
  x = (x - mean) / np.sqrt(var + 0.00001)  # normalize x to have mean=0 and var=1 over last axis
  return gama*x + beta

#layerNormalisaition(x,0.1,0.1)
#clasic linear function
def linear(x,w,b):
  return x @ w + b


#Feed Forward network consist of one layer of neurons and linearised inputs and outputs
#In and out consist of a weihts and biases then decompose tupple **(wIn,b)
def feedForwardNetwork(x,wIn,wOut,bIn,bOut):
  middleRepresentation = activationFunction(linear(x,wIn,bIn))
  return linear(middleRepresentation,wOut,bOut)



#Atention mechanisam multiplays query and k.T then meen it on number of query dimension @ v

def attention(q,k,v,mask):
  print(q)
  print(k)
  print(v)
  print(mask)
  return softmax(q @ k.T / np.sqrt(q.shape[-1]) ) @ v

def multiHeadAttention(x,wIn,wOut,bIn,bOut,nHeads):

    x = linear(x,wIn,bIn)
    #qkv 3 sub array [q], [k],[v]
    qkv = np.split(x, 3, axis=-1)
    #Geting heads
    qkv_heads = list(map(lambda x: np.split(x, nHeads), qkv))
    print(qkv)
     # causal mask to hide future inputs from being attended to
    causal_mask = (1 - np.tri(x.shape[0], dtype=x.dtype)) * -1e10

    # perform attention over each head
    out_heads = [attention(q, k, v, causal_mask) for q, k, v in zip(*qkv_heads)]

    # merge heads
    x = np.hstack(out_heads)
    # out projection
    x = linear(x, wOut, bOut)

    return x


def transformerBlock(x, mlpwIn, mlpwOut, mlpbIn, mlpbOut, wIn, wOut, bIn, bOut, gama, beta, n_head):
    # multi-head causal self attention
    x = x + multiHeadAttention(layerNormalisaition(x, gama, beta), wIn, wOut, bIn, bOut , n_head)
    #causal_mask = (1 - np.tri(x.shape[0], dtype=x.dtype)) * -1e10
    #x = x + attention(layerNormalisaition(x, gama, beta),wIn,wOut,causal_mask)
    # position-wise feed forward network
    x = x + feedForwardNetwork(layerNormalisaition(x, gama, beta), mlpwIn, mlpwOut, mlpbIn, mlpbOut,)
    
    return x


mlpwIn =  mlpwOut =  mlpbIn =  mlpbOut = np.ones((3))
print(mlpwIn)
wIn =  wOut =  bIn = bOut  = np.ones((3))
gama = 1
beta = 0 
nHead = 0
x = np.ones(3)


a = transformerBlock(x, mlpwIn, mlpwOut, mlpbIn, mlpbOut, wIn, wOut, bIn, bOut, gama, beta, nHead)
print(a)
