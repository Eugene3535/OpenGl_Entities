#pragma once

#include <glad/glad.h>

#include "Texture.hpp"
#include "Sprite.hpp"
#include "Geometry.hpp"

#include <string_view>
#include <vector>
#include <map>

// Single animation data. For example, character one side stand/walk cycle
struct AnimationData
{
	// Drops important options by default
	void reset() 
	{
		current_frame = elapsed_time = 0;
	}

	bool                is_looped     = false;
	std::size_t         current_frame = 0;
	std::size_t         end           = 0;
	float               delay         = 0;
	float               elapsed_time  = 0;
	std::vector<HitBox> frames;
};

// Sprite with only one linear animation
class AnimatedSprite: public Sprite
{
public:
	AnimatedSprite(Texture* texture, GLuint x, GLuint y, GLuint width, GLuint height, GLuint duration, float delay, bool loop = false):
		Sprite()
	{
		data.end       = duration;
		data.delay     = delay;
		data.is_looped = loop;
		data.frames.reserve(duration);

		for (std::size_t i = 0; i < duration; ++i)
			data.frames.emplace_back(std::move(HitBox(x + i * width, y, width, height)));

		setTexture(texture);
		setTextureRect(data.frames[0]);
	}

	void tick(float delta_time)
	{
		data.elapsed_time += delta_time;

		if (data.elapsed_time >= data.delay)
		{
			data.elapsed_time = 0;
			data.current_frame++;

			if (isEnd())			
				if (!data.is_looped)			
					return;			
				else
					data.current_frame = 0;
			
			setTextureRect(data.frames[data.current_frame]);
		}
	}

	void reset()
	{
		data.reset();
		setTextureRect(data.frames[0]);
	}

	bool isEnd() const
	{
		return data.current_frame == data.end;
	}

private:
	AnimationData data;
};

// Sprite with multiple animation. Important! Animation playing cycle sets by manual control (play/pause) ! 
class AnimationManager : public Sprite
{
public:
	AnimationManager(): Sprite(), is_playing(false)
	{
	}

	~AnimationManager()
	{
	}

	void add(const std::string_view name, GLuint x, GLuint y, GLuint width, GLuint height, GLuint duration, float delay)
	{
		AnimationData data;
		data.end   = duration;
		data.delay = delay;
		data.frames.reserve(duration);

		for (std::size_t i = 0; i < duration; ++i)
			data.frames.emplace_back(std::move(HitBox(x + i * width, y, width, height)));
		
		anim_list.emplace(name, std::move(data));
		set(name);
	}

	void set(const std::string_view name)
	{
		if(current_anim != name)
		//if (anim_list.find(name) != anim_list.end())
		{
			anim_list[current_anim].reset();
			current_anim = name;
			setTextureRect(anim_list[current_anim].frames[0]);
		}			
	}

	void tick(float delta_time)
	{
		if (!is_playing) return;

		AnimationData& anim = anim_list[current_anim];
		anim.elapsed_time += delta_time;

		if (anim.elapsed_time >= anim.delay)
		{
			anim.elapsed_time = 0;
			anim.current_frame++;

			if (anim.current_frame == anim.end)		
				anim.current_frame = 0;
			
			setTextureRect(anim.frames[anim.current_frame]);
		}
	}

	void play()
	{
		is_playing = true;
	}

	void pause()
	{
		is_playing = false;
	}

	bool isPlaying() const
	{
		return is_playing;
	}

private:
	bool is_playing;
	std::string_view current_anim;
	std::map<std::string_view, AnimationData> anim_list;
};