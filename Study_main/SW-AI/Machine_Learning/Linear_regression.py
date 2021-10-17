# Comments with #
import numpy as np  # For doing math
from matplotlib import pyplot as plt  # For plotting

from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt

# Hypothesis
# If we have d labels, we expect x to be of length d+1, with x[0]=1.
# In the following we just consider d = 1
def hypothesis(theta, x):
    ret = 0
    # please implement the hypothesis equation shown in p16, W5-1.pdf
    # ...
    # ...
    ret = theta[0] + theta[1] * x
    return ret

# The cost function J(theta)
def cost_function(X, y, theta):
    ret = 0
    # n: Number of training examples
    n = len(X)
    y_hat = hypothesis(theta, X)
    sse = y - y_hat
    ret = sse ** 2
    ret = ret.sum() / 2
    # please implement the cost function using hypothesis function above shown in p8, W5-2.pdf
    # ...
    # ...
    return(ret)

def comparison_cost(X, y, t1, t0):
    cost = cost_function(X, y, [t0, t1])
    # Evaluate the cost function as some values
    print("Cost for minimal parameters:", cost, ", with theta0 =", t0,
          " and theta1 =", t1, )  # Best possible (got it from numpy)
    print("Cost for other theta:", cost_function(X, y, [8.4, 0.6]))

def gradient(X, y, theta):
    g = np.array([0., 0.])
    n = len(X)
    y_hat = hypothesis(theta,X)
    sse = y - y_hat
    g[0] = sse.sum()
    g[1] = (sse * X).sum()
    # implement the update rule discribed in p24, W5-2.pdf
    #for j in range(n):
    # ...
    # ...
    return(g / np.linalg.norm(g))

def stochastic_gradient(X, y, theta):
    batch = 8
    g = np.array([0, 0])
    n = len(X)
    idx = np.random.choice(n, batch)
    batch_X = X[idx]
    batch_y = y[idx]
    y_hat = hypothesis(theta, batch_X)
    sse = batch_y - y_hat
    g[0] = sse.sum()
    g[1] = (sse * batch_X).sum()
    return(g / np.linalg.norm(g))

def optimise_by_gradient_descent(X, y, t1, t0):
    # Start with some value for theta
    theta = np.array([t0, t1])

    listt0 = np.array([])
    listt1 = np.array([])

    # learning rate
    alpha = 0.001

    # number of steps
    steps = 100

    # gradient descent
    for s in range(steps):
        listt0 = np.append(listt0, theta[0])  # save data for drawing
        listt1 = np.append(listt1, theta[1])
        
        g = stochastic_gradient(X, y, theta)
#         g = gradient(X, y, theta)
        theta[0] = theta[0] + alpha * g[0]
        theta[1] = theta[1] +alpha * g[1]
        # parameter update part - Using gradient function, please implement the parameter update
        # as described in the algorithm in p21, W5-2.pdf
        # ...
        # ...

    print("Gradient descent gives after", steps, "steps: ", theta)
    print("Best theta: ", [t0, t1])
    return listt0, listt1

def main():

    # Training set
    Tx = np.array([2, 7, 13, 16, 22, 27, 35, 45, 50])
    Ty = np.array([5, 20, 14, 32, 22, 38, 39, 59, 70])

    # Draw the Training set
    plt.figure(figsize=(10, 8))
    plt.plot(Tx, Ty, 'X')
    plt.title("Training set", fontsize=20)
    plt.xlabel("Weeks living in Jeju", fontsize=18)
    plt.ylabel("# of having black-pork ", fontsize=18)

    # Best fit (by using the built-in function of numpy)
    # This is what we want to find by ourself in the following
    t1, t0 = np.polyfit(Tx, Ty, 1)
    plt.plot(Tx, t0 + t1*Tx)
    print("theta0 :", t0, " theta1:", t1)
    plt.show()

    # HW part
    # t1, t0 is the fitted parameter which stands for the best possible (got it from numpy)
    comparison_cost(Tx, Ty, t1, t0)

    l1,l2 = optimise_by_gradient_descent(Tx, Ty, t1, t0)

    plt.subplot(211)
    plt.plot(l1,label = 't0')
    plt.title('t0')
    plt.subplot(212)
    plt.plot(l2,label = 't1')
    plt.title('t1')
    plt.show()
if __name__ == '__main__':
    main()