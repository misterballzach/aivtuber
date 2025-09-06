from pythonosc import udp_client

class Animator:
    def __init__(self, ip="127.0.0.1", port=9000):
        self.client = udp_client.SimpleUDPClient(ip, port)
        print(f"Animator initialized. Sending OSC messages to {ip}:{port}")

    def send_mouth_open(self, value: float):
        """
        Sends the mouth open value to a common VRChat blendshape address.
        """
        if not 0.0 <= value <= 1.0:
            print("Warning: mouth open value should be between 0.0 and 1.0")
            value = max(0.0, min(1.0, value))

        print(f"Sending OSC message: /vrc/blendshape/mouth_open {value}")
        self.client.send_message("/vrc/blendshape/mouth_open", value)

    def trigger_animation(self, animation_name: str):
        """
        Triggers a named animation.
        """
        print(f"Sending OSC message: /vrc/animation/{animation_name} 1")
        self.client.send_message(f"/vrc/animation/{animation_name}", 1)
        # We send a 0 shortly after to reset the trigger, which is common practice
        # This part might need to be handled differently depending on the Unity setup
        # For now, we'll just send the trigger
        # import time
        # time.sleep(0.1)
        # self.client.send_message(f"/vrc/animation/{animation_name}", 0)

if __name__ == '__main__':
    # Example usage:
    animator = Animator()

    # Test sending a mouth open value
    animator.send_mouth_open(0.75)

    # Test triggering an animation
    animator.trigger_animation("wave")
