#pragma once
#include<string>
#include <SDL.h>
#include <vector>
#include <glm/glm.hpp>
#include <map>
#include "../Atlas.h"
#include "Panel.h"
#include "../BatchRenderer.h"

class States
{
public:
virtual void Update(double deltams) = 0;
virtual void Render() = 0;
virtual void PollEvent(const SDL_Event& ev) = 0;
virtual bool Enter() = 0;
virtual bool Exit() = 0;
virtual std::string getStateID() const { return ID; }
protected:
	std::string ID;
};


class Timer
{
public:
	Timer(double pDuration);
	void Update(double deltams);

	bool IsDone();
	Timer Finish(std::function<void()> pFunc);
	Timer Tween(float* pBegin, float pEnd);
	Timer Chain(std::vector<Timer> pChains);

private:
	bool m_Done;
	double m_Duration;
	double m_Timer;
	float m_Begin, m_End;
	float* m_ValuePtr;

	std::function<void()> m_Func;
	std::vector<Timer> m_Chains;
};


enum BlockType
{
	Null = 0,
	Black_Circle,
	Blue_Diamond,
	Green_Gogone,
	Grey_Octagone,
	Orange_Trigone,
	Pink_Heart,
	Red_Square,
	Yellow_Star
};

struct Block
{
	BlockType type;
	glm::vec2 Position;
	uint32_t background_sprite;
	uint32_t foreground_sprite;

	bool blocked;
};

class Button : public Panel
{
public:
	Button(SDL_Rect pRect, std::string pText, std::function<void()> pFunc, uint32_t idle, uint32_t hover, uint32_t click);
	void Draw() override;
	bool Hover(int x, int y) override;

private:
	bool hovered = false;
	bool click = false;
	bool idle = true;

	Rect rect;
	std::string m_Text;
	std::function<void()> m_Func;

	uint32_t idleId;
	uint32_t hoverId;
	uint32_t clickId;
};

class Test : public States
{
public:
  void Update(double deltams) override;
  void Render()  override;
  void PollEvent(const SDL_Event& ev) override;
  bool Enter()  override;
  bool Exit()  override;
  Test() {
	  ID = "Test";
  }
  ~Test() {
	  
  }
private:

	void DrawBoard(glm::vec2 Offset);

	std::vector<Block> match_table;

	int block_count;
	size_t WIDTH, HEIGHT;

	float block_size;

	glm::vec2 table_offset;

	glm::ivec2 Cursor, Select;

	std::vector<Timer> Transitions;

	std::vector<size_t> Matches;

	std::vector<std::pair<BlockType, uint32_t>> block_type;

	float match_score;
	bool score_count = false;

	Rect table_rect;

	bool CheckMatch();

	//
	ParticleProp prop;

	uint32_t select_background_frame;

	glm::vec2 shutter_size;

	Button* resume_button;
	Button* menu_button;

	float scale;
	bool resume_state;

	int minutes;
	int second;
	int hour;
	int millisecond;
};



class Menu : public States
{
public:
	void Update(double deltams) override;
	void Render()  override;
	void PollEvent(const SDL_Event& ev) override;
	bool Enter()  override;
	bool Exit()  override;
	Menu() {
		ID = "MENU";
	}
	~Menu() {

	}
private:

	Button* button;
	Button* exit_button;
	float scale;

	Timer* sound_loop;

	Timer* timer;

	void Play();

};