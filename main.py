# Single-file VTuber Bot
# This file is a combination of all modules, created as a workaround for import issues in the execution environment.

import asyncio
import io
import json
import aiohttp
from gtts import gTTS
from pydub import AudioSegment
from pydub.playback import play
from pythonosc import udp_client
from twitchAPI.chat import Chat, EventData, ChatMessage

# --- Class Definitions ---

class ChatBot:
    def __init__(self, api_key):
        self.api_key = api_key
        self.api_url = f"https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key={self.api_key}"
        self.session = aiohttp.ClientSession()
    async def send_message(self, message):
        headers = {'Content-Type': 'application/json'}
        data = {"contents": [{"parts": [{"text": message}]}]}
        try:
            async with self.session.post(self.api_url, headers=headers, json=data) as response:
                response.raise_for_status()
                response_json = await response.json()
                return response_json['candidates'][0]['content']['parts'][0]['text']
        except Exception as e:
            print(f"Error communicating with Gemini: {e}")
            return "Sorry, I'm having trouble thinking right now."
    async def close(self):
        await self.session.close()

class TTSEngine:
    def __init__(self, language='en'):
        self.language = language
    def text_to_audio_stream(self, text):
        try:
            audio_stream = io.BytesIO()
            tts = gTTS(text=text, lang=self.language, slow=False)
            tts.write_to_fp(audio_stream)
            audio_stream.seek(0)
            return audio_stream
        except Exception as e:
            print(f"An error occurred during TTS conversion: {e}")
            return None
    async def play_audio_stream(self, audio_stream):
        if audio_stream:
            try:
                sound = AudioSegment.from_file(audio_stream, format="mp3")
                await asyncio.to_thread(play, sound)
            except Exception as e:
                print(f"An error occurred during audio playback: {e}")

class Animator:
    def __init__(self, ip, port):
        self.client = udp_client.SimpleUDPClient(ip, port)
        print(f"Animator initialized. Sending OSC to {ip}:{port}")
    def trigger_animation(self, animation_name: str):
        print(f"Sending OSC: /vrc/animation/{animation_name} 1")
        self.client.send_message(f"/vrc/animation/{animation_name}", 1)

class TwitchBot:
    def __init__(self, token, nickname, channel, response_callback):
        self.token = token
        self.nickname = nickname
        self.channel = channel
        self.response_callback = response_callback
        self.chat = Chat(self.token, self.nickname)
        self.chat.register_event('MESSAGE', self.on_message)
    async def on_message(self, msg: ChatMessage):
        if msg.text.startswith('!bot '):
            user_message = msg.text[5:]
            print(f"Received command from {msg.user.name}: {user_message}")
            asyncio.create_task(self.response_callback(user_message))
    async def start(self):
        self.chat.start()
        await self.chat.join_room(self.channel)
        print("Twitch bot connected.")
    async def stop(self):
        self.chat.stop()

# --- Main Application Logic ---

async def main(config):
    chatbot = ChatBot(api_key=config.GEMINI_API_KEY)
    tts_engine = TTSEngine()
    animator = Animator(ip=config.OSC_IP, port=config.OSC_PORT)
    response_lock = asyncio.Lock()

    async def handle_response(user_message):
        async with response_lock:
            print(f"--- Handling response for: {user_message} ---")
            bot_response = await chatbot.send_message(user_message)
            print(f"Bot Response: {bot_response}")
            animator.trigger_animation("talking_start")
            audio_stream = tts_engine.text_to_audio_stream(bot_response)
            await tts_engine.play_audio_stream(audio_stream)
            animator.trigger_animation("talking_end")
            print("--- Finished handling response ---")

    twitch_bot = TwitchBot(
        token=config.TWITCH_TOKEN,
        nickname=config.TWITCH_NICKNAME,
        channel=config.TWITCH_CHANNEL,
        response_callback=handle_response
    )
    await twitch_bot.start()
    try:
        print("VTuber Bot is running. Press Ctrl+C to stop.")
        await asyncio.Event().wait()
    finally:
        await twitch_bot.stop()
        await chatbot.close()
        print("VTuber Bot Shut Down.")

# --- Application Entry Point ---

if __name__ == "__main__":
    print("Starting VTuber Bot...")
    config = None
    try:
        import config
    except ImportError:
        print("ERROR: config.py not found. Please create it from config.py.example and fill in your credentials.")
        exit(1)

    if config.GEMINI_API_KEY == "YOUR_GEMINI_API_KEY" or config.TWITCH_TOKEN == "oauth:your_twitch_oauth_token":
        print("ERROR: Credentials not found in config.py. Please fill in your details.")
        exit(1)

    try:
        asyncio.run(main(config))
    except KeyboardInterrupt:
        print("Bot stopped by user.")
