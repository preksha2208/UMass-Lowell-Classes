import pygame
from pygame.locals import *

width = 15
height = 80

ballX = 300 #center of ball, not corners
ballY = 300
ballRadius = 20
ballVelX = 1
ballVelY = 1

scoreA = 0
scoreB = 0

aiY = 300

white = (255, 255, 255)

pygame.init()

screen = pygame.display.set_mode((600, 600))
pygame.display.set_caption("Pong")

font = pygame.font.Font(None, 32)
text = font.render('Player 1: 0   Player 2: 0', True, white, None)
scoreboard = text.get_rect()
scoreboard.center = (300, 550)

clock = pygame.time.Clock()
running = True

while running:
    clock.tick(60)
    screen.blit(text, scoreboard)
    #logic
    ballX += ballVelX * 3
    ballY += ballVelY * 3

    aiY = ballY

    if ballX + ballRadius > 600:
        ballVelX = -1
    elif ballX + ballRadius < 0:
        ballVelX = 1
    
    if ballY + ballRadius > 600:
        ballVelY = -1
    elif ballY + ballRadius < 0:
        ballVelY = 1

    if ballY > pygame.mouse.get_pos()[1] - height/2: 
        if ballY < pygame.mouse.get_pos()[1] + height/2:   # if the ball is within the Y dimensions of the paddle
            if ballX - ballRadius <= 10 + width:   # check whether the ball is within the paddle's x dimensions
                 ballVelX = 1
    
    if ballY > aiY - height/2:
        if ballY < aiY + height/2:   # if the ball is within the Y dimensions of the paddle
            if ballX + ballRadius >= 600 - 10 - width:   # check whether the ball is within the paddle's x dimensions
                ballVelX = -1



    #rendering
    screen.fill((0,0,0))
    pygame.draw.rect(screen, white, (10, pygame.mouse.get_pos()[1] - height/2, width, height)) # rectangles in pygame defined by (x,y,width, height), ((x,y,), (width, height))
    pygame.draw.rect(screen, white, (600 - 10 - width, aiY - height/2, width, height)) # rectangles in pygame defined by (x,y,width, height), ((x,y,), (width, height))
    pygame.draw.circle(screen, white, (ballX, ballY), ballRadius)
    pygame.display.flip()
    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit() 
            running = False