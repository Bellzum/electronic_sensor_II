"""mqtt_bridge_node.py - ROS 2 node that mirrors Mini Smart World MQTT topics into ROS.

Purpose:
    MQTT world/<zone>/<thing>  ->  ROS /<zone>/<thing>   (std_msgs/String, raw text or JSON)
    ROS  /safe_cmd/<zone>      ->  MQTT world/cmd/<zone>  (only commands the safety node allowed)
    ROS  /estop (std_msgs/Bool) -> MQTT world/estop

Starter code for Control Center v2. Run inside a ROS 2 workspace (see README.md).
"""
import paho.mqtt.client as mqtt
import rclpy
from rclpy.node import Node
from std_msgs.msg import Bool, String

MIRRORED = ["gate/state", "gate/card", "security/state", "security/alarm", "lights/state",
            "farm/sensors", "farm/state", "tower/position", "tower/event",
            "rover/state", "dog/state", "dog/sensors"]
ZONES = ["farm", "tower", "rover", "dog"]


class MqttBridge(Node):
    """Purpose: one node that owns the MQTT connection and a ROS topic per world topic."""

    def __init__(self):
        super().__init__("mqtt_bridge_node")
        host = self.declare_parameter("mqtt_host", "localhost").value
        self.mqtt = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="ros-mqtt-bridge")
        self.ros_pubs = {name: self.create_publisher(String, "/" + name, 10) for name in MIRRORED}
        for zone in ZONES:
            self.create_subscription(String, f"/safe_cmd/{zone}",
                                     lambda msg, z=zone: self.mqtt.publish(f"world/cmd/{z}", msg.data), 10)
        self.create_subscription(Bool, "/estop",
                                 lambda msg: self.mqtt.publish("world/estop", "true" if msg.data else "false", retain=True), 10)
        self.mqtt.on_message = self.on_mqtt
        self.mqtt.connect(host, 1883)
        self.mqtt.subscribe("world/#")
        self.mqtt.loop_start()
        self.get_logger().info(f"bridging MQTT {host} <-> ROS")

    def on_mqtt(self, client, userdata, msg):
        """Purpose: forward one MQTT message to its ROS topic.

        Args:
            client, userdata, msg: MQTT callback arguments
        Returns:
            None
        """
        name = msg.topic.removeprefix("world/")
        if name in self.ros_pubs:
            self.ros_pubs[name].publish(String(data=msg.payload.decode(errors="ignore")))


def main():
    """Purpose: start the node. Args: none. Returns: None."""
    rclpy.init()
    node = MqttBridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.mqtt.loop_stop()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
