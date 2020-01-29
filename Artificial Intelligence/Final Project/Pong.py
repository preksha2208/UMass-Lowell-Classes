import turtle

wn =  turtle.Screen()
wn.title("Pong")
wn.bgcolor("black")
wn.setup(width = 800, height = 600)
wn.tracer(0)


#Main game loop (every game needs this)
while True:
    wn.update()