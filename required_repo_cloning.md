## Adding The Required Repositories.

##Simulation Repository

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

# Guidance Navigation and Control Repository


## Clone the IQ GNC ROS package

First, we will clone the IQ GNC ROS package. This ROS package comes with my GNC API that will make scripting the drone easy. It will also come with a worked out solution to this tutorial 
```
git clone https://github.com/Intelligent-Quads/iq_gnc.git
```

this package contains the file `gnc_functions.hpp`. This file contains a bunch of useful functions for creating intelligent drone applications.

