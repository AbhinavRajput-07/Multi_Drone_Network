## Adding The Required Repositories.

## Clone IQ Simulation ROS package 

```
cd ~/catkin_ws/src
git clone https://github.com/Intelligent-Quads/iq_sim.git
```

run the following to tell gazebo where to look for the iq models.
```
echo "GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:$HOME/catkin_ws/src/iq_sim/models" >> ~/.bashrc
```

## Build instructions
Inside `catkin_ws`, run `catkin build`:

```
cd ~/catkin_ws
catkin build
```
update global variables
```
source ~/.bashrc
```
