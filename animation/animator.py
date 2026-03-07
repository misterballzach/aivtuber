import threading
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

    def trigger_animation(self, animation_name: str, reset_trigger: bool = True, reset_delay: float = 0.1):
        """
        Triggers a named animation.
        Optionally resets the trigger after a delay, which is common practice.
        """
        print(f"Sending OSC message: /vrc/animation/{animation_name} 1")
        self.client.send_message(f"/vrc/animation/{animation_name}", 1)

        if reset_trigger:
            def reset():
                print(f"Sending OSC message: /vrc/animation/{animation_name} 0 (reset)")
                self.client.send_message(f"/vrc/animation/{animation_name}", 0)

            timer = threading.Timer(reset_delay, reset)
            timer.start()

if __name__ == '__main__':
    # Example usage:
    animator = Animator()

    # Test sending a mouth open value
    animator.send_mouth_open(0.75)

    # Test triggering an animation
    animator.trigger_animation("wave")
