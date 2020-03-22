import gym
from gym import error, spaces, utils
from gym.utils import seeding

class connect4(gym.Env):
  metadata = {'render.modes': ['human']}

  def __init__(self):
    print("init success")
  def step(self, action):
    print("step success")
  def reset(self):
    print("reset success")
  def render(self, mode='human', close=False):
    print("render success")