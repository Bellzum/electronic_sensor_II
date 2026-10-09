# Control Center v2 · Gazebo digital twin (weeks 15–17)

A **digital twin** is a simulated copy of the real thing. Here: the rover and the road, in Gazebo.

## Words first
- **Gazebo:** a physics simulator (use the current Gazebo, not "Gazebo Classic").
- **URDF / SDF:** text files that describe a robot's body (links and joints) or a world.
- **ros_gz bridge:** connects Gazebo topics to ROS 2 topics.

## Plan
1. Run a ready-made differential-drive robot demo from the Gazebo + ROS 2 tutorials.
2. Make a world file with a flat road: 4–6 boxes 0.30 × 0.10 m (same size as the cardboard tiles) and a wall at the "gate".
3. Drive it with `teleop_twist_keyboard` → `/cmd_vel`.
4. Put `world_safety_node` in between: keyboard → `/cmd/rover` → safety → `/cmd_vel`.
5. Add a simulated distance sensor; check the robot stops 20 cm before the gate wall.
6. Compare with the real rover: write in `../experiments/test_log.csv` what the simulation gets wrong (real sensors are noisy, real wheels slip).

| | Build | Status |
|---|---|---|
| v1 | Demo robot driving in Gazebo | ☐ |
| v2 | Road world with gate wall | ☐ |
| v3 | Safety node stops the simulated rover at the wall | ☐ |
| v4 | Sim vs real comparison written | ☐ |
