# Controlling Multiple Ardupilot Drones Via MAVROS


## Executing a Multi-Drone Swarm Mission

In this example we will use  `square.cpp` program in conjunction with the `multi_drone.world` gazebo sim.

first make a copy of square.cpp and call it multi_square.cpp.

We will be creating a drone swarm where each drone flies a synchronized square pattern. Instead of having to change each drone to GUIDED manually, we will be having the program initiate on start. To do this we will be replacing `wait4start();` with `set_mode("GUIDED");`. This is the only code change that will be needed. Otherwise we may have to encounter some delay.

Next we are going to setup a new launch file to launch each drone's gnc code. Create a new file in launch called `multi_square.launch` 
```
<launch>
	<group>
		<node pkg="iq_gnc" type="square" name="square1" output="screen" ns="/drone1">
			<param name="namespace" value="/drone1"/>
			<param name="use_sim_time"  value="true" />
		</node>
	</group>

	<group>
		
		<node pkg="iq_gnc" type="square" name="square2" output="screen" ns="/drone2">
			<param name="namespace" value="/drone2"/>
			<param name="use_sim_time"  value="true" />
		</node>
	</group>
	<group>	
		<node pkg="iq_gnc" type="square" name="square3" output="screen" ns="/drone3">
			<param name="namespace" value="/drone3"/>
			<param name="use_sim_time"  value="true" />
		</node>
	</group>
</launch>
```

you will notice the below line 
```
<param name="namespace" value="/drone4"/>
```
this parameter tells the gnc_functions what namespace this program's mavros topics fall under. 

The next thing we will need to do is setup a mavros launch file to create the needed links between the drone and the companion computer ros environment. 

creating the file `multi-am.launch` within `iq_sim/launch` 

add the below 
```
<launch>
	<node pkg="mavros" type="mavros_node" name="mavros" required="false" clear_params="true" output="screen" respawn="true" ns="/drone1">
		<param name="fcu_url" value="udp://127.0.0.1:14551@14555" />
		<param name="gcs_url" value="" />
		<param name="target_system_id" value="1" />
		<param name="target_component_id" value="1" />
		<param name="fcu_protocol" value="v2.0" />

		<!-- load blacklist, config -->
		<rosparam command="load" file="$(find mavros)/launch/apm_pluginlists.yaml" />
		<rosparam command="load" file="$(find mavros)/launch/apm_config.yaml" />
	</node>

	<node pkg="mavros" type="mavros_node" name="mavros" required="false" clear_params="true" output="screen" respawn="true" ns="/drone2">
		<param name="fcu_url" value="udp://127.0.0.1:14561@14565" />
		<param name="gcs_url" value="" />
		<param name="target_system_id" value="2" />
		<param name="target_component_id" value="1" />
		<param name="fcu_protocol" value="v2.0" />

		<!-- load blacklist, config -->
		<rosparam command="load" file="$(find mavros)/launch/apm_pluginlists.yaml" />
		<rosparam command="load" file="$(find mavros)/launch/apm_config.yaml" />
	</node>
	<node pkg="mavros" type="mavros_node" name="mavros" required="false" clear_params="true" output="screen" respawn="true" ns="/drone3">
		<param name="fcu_url" value="udp://127.0.0.1:14571@14575" />
		<param name="gcs_url" value="" />
		<param name="target_system_id" value="3" />
		<param name="target_component_id" value="1" />
		<param name="fcu_protocol" value="v2.0" />

		<!-- load blacklist, config -->
		<rosparam command="load" file="$(find mavros)/launch/apm_pluginlists.yaml" />
		<rosparam command="load" file="$(find mavros)/launch/apm_config.yaml" />
	</node>
</launch>
```

- we will notice that the `fcu_url` increments by 10 for each corresponding udp port for the drones. 
- the `target_system_id` increments by 1 for each instance. 
- we will notice that each node has a unique ns (namespace). this will put a unique topic prefix for each mavros topic

You will notice that launching each ardupilot instance might be some more work for repetitive use, so it might be helpful to create the below shell script as `multi-ardupilot.sh`

```
#!/bin/bash

gnome-terminal \
 --tab -e "sim_vehicle.py -v ArduCopter -f gazebo-drone1 -I0" \
 --tab -e "sim_vehicle.py -v ArduCopter -f gazebo-drone2 -I1" \
 --tab -e "sim_vehicle.py -v ArduCopter -f gazebo-drone3 -I2" \
```

## Running the Program

Run the gazebo sim 
```
roslaunch iq_sim multi_drone.launch
```
Run the ardupilot instances
```
./multi-ardupilot.sh
```
Run the mavros instances
```
roslaunch iq_sim multi-apm.launch 
```
Run the guidance program 
```
roslaunch iq_gnc multi_square.launch
```


