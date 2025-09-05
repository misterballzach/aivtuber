import os
import requests
import json
from tts.tts_engine import TTSEngine

class ChatBot:
    def __init__(self, api_key):
        self.api_key = api_key
        self.api_url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent"

    def send_message(self, message):
        headers = {
            'Content-Type': 'application/json',
            'X-goog-api-key': self.api_key,
        }

        data = {
            "contents": [
                {
                    "parts": [
                        {
                            "text": message
                        }
                    ]
                }
            ]
        }

        try:
            response = requests.post(self.api_url, headers=headers, data=json.dumps(data))
            response.raise_for_status()  # Raise an exception for bad status codes
            response_json = response.json()
            # Extract the text from the response.
            # The actual path might be different, you may need to inspect the response to find the correct path.
            # Based on the Gemini API documentation, the response should have a 'candidates' field.
            return response_json['candidates'][0]['content']['parts'][0]['text']
        except requests.exceptions.RequestException as e:
            return f"Error: {e}"
        except (KeyError, IndexError) as e:
            return f"Error parsing response: {e}"

if __name__ == '__main__':
    # IMPORTANT: Replace "YOUR_API_KEY" with your actual Google AI API key.
    api_key = "YOUR_API_KEY"

    if api_key == "YOUR_API_KEY":
        print("Please replace 'YOUR_API_KEY' with your actual Google AI API key.")
    else:
        bot = ChatBot(api_key=api_key)
        tts_engine = TTSEngine()
        print("Chatbot initialized. Type 'quit' to exit.")
        while True:
            user_input = input("You: ")
            if user_input.lower() == 'quit':
                break
            response = bot.send_message(user_input)
            print(f"Bot: {response}")
            audio_stream = tts_engine.text_to_audio_stream(response)
            tts_engine.play_audio_stream(audio_stream)
