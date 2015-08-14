#include "AudioManager.h"
#include "Log.h"

namespace Ryno{
	AudioManager::AudioManager()
	{
	}


	AudioManager::~AudioManager()
	{
		destroy();
	}


	void AudioManager::init(){
		if (m_is_initialized){
			Log::FatalError("Tried to initialize audio engine twice");

		}
		//take bitwise with MIX_INIT_formato
		if (Mix_Init(MIX_INIT_MP3) == -1){
			Log::FatalError("SDL_Mixer failed: " + std::string(Mix_GetError()));
		}

		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024)){
			Log::FatalError("SDL_Mixer failed: " + std::string(Mix_GetError()));
		}

		m_is_initialized = true;
	}
	void AudioManager::destroy(){
		if (m_is_initialized){
			m_is_initialized = false;
			for (auto& it : m_sound_map){
				Mix_FreeChunk(it.second);
			}
			for (auto& it : m_music_map){
				Mix_FreeMusic(it.second);
			}

			m_sound_map.clear();
			m_music_map.clear();
			Mix_CloseAudio();//opposite of openaudio
			Mix_Quit();//opposite of init
		}
	}

	Sound AudioManager::load_sound(const std::string& file_path){

		Sound sound;
	
		//check if already cached
		auto it = m_sound_map.find(file_path);
		if (it == m_sound_map.end()){
			//if not load
			Mix_Chunk* chunk = Mix_LoadWAV(std::string("Resources/Sounds/" + file_path).c_str()); //despite name load everything
			if (chunk == nullptr)
				Log::FatalError("Mix_LoadWAV failed: " + std::string(Mix_GetError()));
			//insert it in map
			m_sound_map[file_path] = chunk; //map used as associative array
			sound.m_chunk = chunk;

		}
		else{
			sound.m_chunk = it->second;
		}

		return sound;

	}

	Music AudioManager::load_music(const std::string& file_path){
		Music music;

		//check if already cached
		auto it = m_music_map.find(file_path);
		if (it == m_music_map.end()){
			//if not load
			Mix_Music* mus = Mix_LoadMUS(std::string("Resources/Music/" + file_path).c_str()); //despite name load everything
			if (mus == nullptr)
				Log::FatalError("Mix_LoadMus failed: " + std::string(Mix_GetError()));
			//insert it in map
			m_music_map[file_path] = mus; //map used as associative array
			music.m_music = mus;
		}
		else{
			music.m_music = it->second;
		}
		return music;
	}


	void Sound::play(U32 loops){
		if (Mix_PlayChannel(-1, m_chunk, loops) == -1){
			//if all channels are busy, override first one
			if (Mix_PlayChannel(0, m_chunk, loops) == -1){
				Log::FatalError("Mix_PlayChannel error: " + std::string(Mix_GetError()));
			}
		}
	}

	void Sound::set_volume(F32 volume){
		Mix_Volume(-1, (I32)(volume * 128));
	}
	//loops -1 = infinite
	void Music::play(U32 loops){
		Mix_PlayMusic(m_music, loops);
	}

	void Music::pause(){

		Mix_PauseMusic();

	}

	void Music::stop(){
		Mix_HaltMusic();
	}

	void Music::resume(){

		Mix_ResumeMusic();
	}

	void Music::set_volume(F32 volume){
		Mix_VolumeMusic((I32)(volume * 128));
	}

}