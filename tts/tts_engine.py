from gtts import gTTS
from pydub import AudioSegment
from pydub.playback import play
import io

class TTSEngine:
    def __init__(self, language='en'):
        self.language = language

    def text_to_audio_stream(self, text):
        """
        Converts text to an in-memory audio byte stream.
        """
        try:
            audio_stream = io.BytesIO()
            tts = gTTS(text=text, lang=self.language, slow=False)
            tts.write_to_fp(audio_stream)
            audio_stream.seek(0) # Rewind the stream to the beginning
            return audio_stream
        except Exception as e:
            print(f"An error occurred during text-to-speech conversion: {e}")
            return None

    def play_audio_stream(self, audio_stream):
        """
        Plays an audio stream using pydub.
        """
        if audio_stream:
            try:
                sound = AudioSegment.from_file(audio_stream, format="mp3")
                play(sound)
                return True
            except Exception as e:
                print(f"An error occurred during audio playback: {e}")
        return False

if __name__ == '__main__':
    # Example usage:
    engine = TTSEngine()
    text = "Hello, this is a test of the streaming text to speech engine."

    # 1. Convert text to an audio stream
    audio_stream = engine.text_to_audio_stream(text)

    # 2. Play the audio stream
    if audio_stream:
        print("Playing audio stream...")
        engine.play_audio_stream(audio_stream)
        print("Playback finished.")
