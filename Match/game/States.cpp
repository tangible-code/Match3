#include "States.h"
#include "StateManager.h"
#include "../Application.h"
#include <fstream>
#include <math.h>
#include <nlohmann/json.hpp>

bool shutter_on = false;

double shutter_time = 1.2;

void Test::Update(double deltams)
{

	millisecond += int((1.0 / deltams) * 1000.0);

	if (millisecond > 1000)
	{
		millisecond = millisecond - 1000;
		second += 1;
	}
	
	if (second > 60)
	{
		second = second - 60;
		minutes += 1;
	}

	if (minutes > 60)
	{
		minutes = minutes - 60;
		hour += 1;
	}

	for (size_t i = 0; i < Transitions.size(); i++)
	{
		if (!Transitions[i].IsDone())
			Transitions[i].Update(deltams);
	}

	int i = 0;
	while (i < Transitions.size())
	{
		if (Transitions[i].IsDone())
		{
			std::swap(Transitions[i], Transitions.back());
			Transitions.pop_back();

			if (Transitions.empty())
				break;
			continue;
		}

		i++;
	}


	//Matching

	//Horizontal

	if (!shutter_on)
	{
		int Match = 0;
		BlockType type = Null;

		if (Transitions.empty())
		{
			Matches.clear();

			for (size_t y = 0; y < HEIGHT; y++)
			{
				for (size_t x = 0; x < WIDTH; x++)
				{
					if (match_table[(y * WIDTH) + x].blocked)
					{
						if (match_table[(y * WIDTH) + x].type == type)
						{
							Match++;
						}
						else
						{
							if (Match > 2)
							{
								for (size_t i = 1; i <= Match; i++)
								{
									Matches.push_back((y * WIDTH) + (x - i));
								}
							}

							Match = 1;
							type = match_table[(y * WIDTH) + x].type;
						}
					}
					else
					{
						if (Match > 2)
						{
							for (size_t i = 1; i <= Match; i++)
							{
								Matches.push_back((y * WIDTH) + (x - i));
							}
						}

						Match = 1;
						type = Null;
					}
				}

				if (Match > 2)
				{
					for (size_t i = 1; i <= Match; i++)
					{
						Matches.push_back((y * WIDTH) + (WIDTH - i));
					}
				}

				Match = 1;
				type = Null;
			}



			for (size_t x = 0; x < WIDTH; x++)
			{
				for (size_t y = 0; y < HEIGHT; y++)
				{
					if (match_table[(y * WIDTH) + x].blocked)
					{
						if (match_table[(y * WIDTH) + x].type == type)
						{
							Match++;
						}
						else
						{
							if (Match > 2)
							{
								for (size_t i = 1; i <= Match; i++)
								{
									Matches.push_back(((y - i) * WIDTH) + x);
								}
							}

							Match = 1;
							type = match_table[(y * WIDTH) + x].type;
						}
					}
					else
					{
						if (Match > 2)
						{
							for (size_t i = 1; i <= Match; i++)
							{
								Matches.push_back(((y - i) * WIDTH) + x);
							}
						}

						Match = 1;
						type = Null;
					}
				}

				if (Match > 2)
				{
					for (size_t i = 1; i <= Match; i++)
					{
						Matches.push_back(((HEIGHT - i) * WIDTH) + x);
					}
				}

				Match = 1;
				type = Null;
			}

			float added_score = match_score;


			for (size_t i = 0; i < Matches.size(); i++)
			{
				match_table[Matches[i]].blocked = false;
				added_score += 50.0f;
			}

			if (added_score != match_score)
			{
				score_count = true;
				Transitions.push_back(Timer(0.9).Tween(&match_score, added_score).Finish([&]() {score_count = false; }));


				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_int_distribution<std::mt19937::result_type> distribution;
				float limit = (float)std::numeric_limits<uint32_t>::max();


				for (size_t k = 0; k < Matches.size(); k++)
				{
					for (size_t i = 0; i < 5; i++)
					{
						prop.Position = match_table[Matches[k]].Position + glm::vec2(block_size / 2.0f, block_size / 2.0f) + table_offset;
						prop.Position.x += ((float(distribution(mt)) / limit) - 0.5f) * block_size;
						prop.Position.y += ((float(distribution(mt)) / limit) - 0.5f) * block_size;
						BatchRenderer::Emit(prop);
					}
				}
				Audio::Play("confirm", { 0,0,0 });
			}
		}

		if (Transitions.empty())
		{
			size_t x = 0;
			glm::ivec2 lowest = glm::ivec2(-1, -1);
			glm::ivec2 fall_block = glm::ivec2(-1, -1);

			while (x <= WIDTH - 1)
			{
				for (size_t y = HEIGHT - 1; y != -1; y--)
				{

					if (match_table[(y * WIDTH) + x].blocked)
					{
						if (lowest.y != -1)
						{
							fall_block = glm::ivec2(x, y);

							break;
						}
					}
					else
					{
						if (lowest.y == -1)
						{

							lowest = glm::ivec2(x, y);
						}
					}
				}

				if (fall_block.y != -1 && lowest.y != -1)
				{
					glm::vec2 Position = match_table[(fall_block.y * WIDTH) + fall_block.x].Position;
					std::swap(match_table[(fall_block.y * WIDTH) + fall_block.x], match_table[(lowest.y * WIDTH) + lowest.x]);
					Transitions.push_back(Timer(0.6));
					Transitions.back().Tween(&match_table[(lowest.y * WIDTH) + lowest.x].Position.y, match_table[(fall_block.y * WIDTH) + fall_block.x].Position.y);
					match_table[(fall_block.y * WIDTH) + fall_block.x].Position = Position;

				}
				else
				{
					x++;
				}

				lowest = glm::ivec2(-1, -1);
				fall_block = glm::ivec2(-1, -1);

			}

			std::vector<size_t> blank_block;
			std::random_device rd;
			std::mt19937 mt(rd());

			std::uniform_int_distribution<int> index(0, block_count - 1);

			for (size_t x = 0; x < WIDTH; x++)
			{
				for (size_t y = HEIGHT - 1; y != -1; y--)
				{
					if (!match_table[(y * WIDTH) + x].blocked)
					{
						blank_block.push_back((y * WIDTH) + x);
					}
				}

				for (size_t i = 0; i < blank_block.size(); i++)
				{
					Block b;
					size_t in = index(mt);
					b.type = block_type[in].first;
					b.Position = glm::vec2(match_table[blank_block[i]].Position.x, -(block_size * (i + 1)));
					b.background_sprite = ResourceManager::GetTexture("background_frame")->id;
					b.foreground_sprite = block_type[in].second;
					b.blocked = true;

					std::swap(match_table[blank_block[i]], b);
					Transitions.push_back(Timer(0.6));
					Transitions.back().Tween(&match_table[blank_block[i]].Position.y, b.Position.y);

					if (i == blank_block.size() - 1)
					{
						Transitions.back().Finish([&]() {Audio::Play("hit", { 0,0,0 }); });
					}
				}

				blank_block.clear();
			}

		}

	}
	else
	{
		if (resume_state)
		{
			scale = float(((glm::sin((double)SDL_GetTicks() / 1000) + 1.0) / 2.0) * 0.5) + 1.0f;
		}
	}
}

void Test::Render()
{
	if (!shutter_on)
	{
		BatchRenderer::BeginBatch();

		BatchRenderer::DrawTexture(glm::vec2(80.0f, 80.0f), glm::vec2(200.0f, 60.0f), ResourceManager::GetTexture("grey_panel")->id);

		DrawBoard(table_offset);
		BatchRenderer::EndBatch();
		BatchRenderer::Flush();
		if (!score_count)
		{
			Application::g_logic->f_Render->RenderText({ 100.0f,100.0f }, std::to_string(uint32_t(match_score)), ResourceManager::GetFont("thick"), { 179.0f / 255.0f,183.0f / 255.0f,188.0f / 255.0f });
		}
		else
		{
			Application::g_logic->f_Render->RenderText({ 100.0f - 2.0f,100.0f - 2.0f }, std::to_string(uint32_t(match_score)), ResourceManager::GetFont("thick_big"), { 179.0f / 255.0f,183.0f / 255.0f,188.0f / 255.0f });
		}

		Application::g_logic->f_Render->RenderText({ 0,0 }, std::to_string(hour) + "-" + std::to_string(minutes) + "-" + std::to_string(second), ResourceManager::GetFont("thick"), { 0.0f,0.0f,0.0f },0.8f);
	}
	else
	{
		BatchRenderer::BeginBatch();
		BatchRenderer::DrawQuad({ 0.0f,0.0f }, shutter_size, glm::vec4(255.0f / 255.0f, 204.0f / 255.0f, 0.0f, 1.0f));
		BatchRenderer::EndBatch();
		BatchRenderer::Flush();

		if (resume_state)
		{
			glm::vec2 size = Application::g_logic->f_Render->GetSize("Pause", ResourceManager::GetFont("gluten"), scale);
			Application::g_logic->f_Render->RenderText(glm::vec2(Application::width / 2.0f, 100.0f) - glm::vec2(size.x / 1.8f, 0.0f), "Pause", ResourceManager::GetFont("gluten"), { 0.0f,0.0f,0.0f }, scale);
		}
	}
}

void Test::PollEvent(const SDL_Event& ev)
{
	if (Transitions.empty())
	{
		if (ev.type == SDL_KEYUP)
		{
			if (ev.key.keysym.sym == SDLK_RETURN)
			{
			}
		}
		else if (ev.type == SDL_KEYDOWN)
		{
			if (ev.key.keysym.sym == SDLK_ESCAPE) {
				if (!shutter_on)
				{
					Transitions.push_back(Timer(shutter_time).Tween(&shutter_size.y, (float)Application::height));
					shutter_on = !shutter_on;
					Transitions.push_back(Timer(shutter_time).Finish([&]() {Application::g_logic->ui->Push(resume_button);
					Application::g_logic->ui->Push(menu_button); resume_state = true; }));
				}
			}
		}


		if (!shutter_on)
		{

			if (ev.type == SDL_MOUSEBUTTONDOWN)
			{
				if (ev.button.button == SDL_BUTTON_LEFT)
				{
					glm::vec2 mouse_position = glm::vec2(ev.button.x, ev.button.y);
					if (table_rect.Contain(mouse_position))
					{
						glm::vec2 table_m_pos = mouse_position - table_offset;
						glm::ivec2 block_pos = glm::ivec2(table_m_pos.x / block_size, table_m_pos.y / block_size);
						Cursor = block_pos;

						if (match_table[(Cursor.y * WIDTH) + Cursor.x].blocked)
						{

							if (Select.x == -1)
							{
								Select = Cursor;

							}
							else
							{
								if (Select != Cursor && (std::abs(Select.x - Cursor.x) == 1 || std::abs(Select.y - Cursor.y) == 1))
								{
									if (!(std::abs(Select.x - Cursor.x) && std::abs(Select.y - Cursor.y)))
									{

										std::swap(match_table[(Select.y * WIDTH) + Select.x], match_table[(Cursor.y * WIDTH) + Cursor.x]);
										if (CheckMatch())
										{
											glm::vec2 Position = match_table[(Select.y * WIDTH) + Select.x].Position;

											if (std::abs(Select.x - Cursor.x) == 1)
											{
												Transitions.push_back(Timer(0.3)
													.Tween(&match_table[(Select.y * WIDTH) + Select.x].Position.x, match_table[(Cursor.y * WIDTH) + Cursor.x].Position.x));
												Transitions.push_back(Timer(0.3)
													.Tween(&match_table[(Cursor.y * WIDTH) + Cursor.x].Position.x, Position.x));
											}
											else
											{
												Transitions.push_back(Timer(0.3)
													.Tween(&match_table[(Select.y * WIDTH) + Select.x].Position.y, match_table[(Cursor.y * WIDTH) + Cursor.x].Position.y));
												Transitions.push_back(Timer(0.3)
													.Tween(&match_table[(Cursor.y * WIDTH) + Cursor.x].Position.y, Position.y));
											}
										}
										else
										{

											//test
											std::swap(match_table[(Select.y * WIDTH) + Select.x], match_table[(Cursor.y * WIDTH) + Cursor.x]);

											glm::vec2 Position = match_table[(Select.y * WIDTH) + Select.x].Position;

											if (std::abs(Select.x - Cursor.x) == 1)
											{
												Transitions.push_back(Timer(0.3)
													.Tween(&match_table[(Select.y * WIDTH) + Select.x].Position.x, match_table[(Cursor.y * WIDTH) + Cursor.x].Position.x)
													.Chain({ Timer(0.3).Tween(&match_table[(Select.y * WIDTH) + Select.x].Position.x, Position.x) }));
												Transitions.push_back(Timer(0.3)
													.Tween(&match_table[(Cursor.y * WIDTH) + Cursor.x].Position.x, Position.x)
													.Chain({ Timer(0.3).Tween(&match_table[(Cursor.y * WIDTH) + Cursor.x].Position.x, match_table[(Cursor.y * WIDTH) + Cursor.x].Position.x) }));
											}
											else
											{
												Transitions.push_back(Timer(0.3)
													.Tween(&match_table[(Select.y * WIDTH) + Select.x].Position.y, match_table[(Cursor.y * WIDTH) + Cursor.x].Position.y)
													.Chain({ Timer(0.3).Tween(&match_table[(Select.y * WIDTH) + Select.x].Position.y, Position.y) }));
												Transitions.push_back(Timer(0.3).Tween(&match_table[(Cursor.y * WIDTH) + Cursor.x].Position.y, Position.y)
													.Chain({ Timer(0.3).Tween(&match_table[(Cursor.y * WIDTH) + Cursor.x].Position.y, match_table[(Cursor.y * WIDTH) + Cursor.x].Position.y) }));
											}
										}


										Select = glm::ivec2(-1, -1);

									}

								}
								else
								{
									Select = Cursor;
								}
							}
						}
					}
				}
			}
			else if (ev.type == SDL_MOUSEMOTION)
			{
				glm::vec2 mouse_position = glm::vec2(ev.button.x, ev.button.y);
				if (table_rect.Contain(mouse_position))
				{
					glm::vec2 table_m_pos = mouse_position - table_offset;
					glm::ivec2 block_pos = glm::ivec2(table_m_pos.x / block_size, table_m_pos.y / block_size);

					//if (match_table[(block_pos.y * WIDTH) + block_pos.x].blocked)
					//{
					Cursor = block_pos;
					//}
				}
			}
		}
	}
}

bool Test::Enter()
{
	
	block_size = 64.0f;

	WIDTH = 6;
	HEIGHT = 6;

	table_offset = { (Application::width / 2.0f) - ((float(WIDTH)/2.0f) * block_size),(Application::height / 2.0f) - ((float(HEIGHT) / 2.0f) * block_size) };

	table_offset.x += 200.0f;

	match_score = 0;

	match_table.resize(WIDTH*HEIGHT);

	block_type.push_back({ Black_Circle,ResourceManager::GetTexture("black_circle")->id });
	block_type.push_back({ Blue_Diamond,ResourceManager::GetTexture("blue_diamond")->id });
	block_type.push_back({ Green_Gogone,ResourceManager::GetTexture("green_gogone")->id });
	block_type.push_back({ Grey_Octagone,ResourceManager::GetTexture("grey_octagone")->id });
	block_type.push_back({ Orange_Trigone,ResourceManager::GetTexture("orange_trigone")->id });
	block_type.push_back({ Pink_Heart,ResourceManager::GetTexture("pink_heart")->id });
	block_type.push_back({ Red_Square,ResourceManager::GetTexture("red_square")->id });
	block_type.push_back({ Yellow_Star,ResourceManager::GetTexture("yellow_star")->id });


	block_count = 6;

	std::random_device rd;
	std::mt19937 mt(rd());

	std::uniform_int_distribution<int> index(0, block_count - 1);

	for (size_t y = 0; y < HEIGHT; y++)
	{
		for (size_t x = 0; x < WIDTH; x++)
		{
			Block b;
			size_t i = index(mt);
			b.type = block_type[i].first;
			b.Position = glm::vec2(x * block_size, y * block_size);
			b.background_sprite = ResourceManager::GetTexture("background_frame")->id;
			b.foreground_sprite = block_type[i].second;
			b.blocked = true;
			match_table[(y * WIDTH) + x] = b;
		}
	}
	

	Cursor = glm::ivec2(0, 0);
	Select = glm::ivec2(-1, -1);

	
	table_rect.rect.x = (int)table_offset.x;
	table_rect.rect.y = (int)table_offset.y;
	table_rect.rect.w = int(WIDTH * (size_t)block_size);
	table_rect.rect.h = int(HEIGHT * (size_t)block_size);

	prop.ColorBegin = { 1.0f,1.0f,1.0f,1.0f };
	prop.ColorEnd = { 1.0f,1.0f,1.0f,1.0f };
	prop.SizeBegin = 16.0f, prop.SizeVartiation = 8.0f, prop.SizeEnd = 8.0f;
	prop.Lifetime = 1.0f;
	prop.Velocity = { 0.0f,0.0f };
	prop.VelocityVariation = { 0.0f,0.0f };
	//prop.VelocityVariation = { 0.0f,0.0f };
	prop.Position = { 300.0f,300.0f };
	prop.TextureId = ResourceManager::GetTexture("particle")->id;

	//prop.Velocity = glm::normalize(glm::vec2(100.0f, 100.0f) - prop.Position)*100.0f;

	select_background_frame = ResourceManager::GetTexture("blue_background_frame")->id;

	Application::g_renderer->SetClearColor(glm::vec4(20.0f / 255.0f, 151.0f/ 255.0f, 204.0f / 255.0f, 1.0f));

	shutter_on = true;

	shutter_size = glm::vec2(Application::width, Application::height);

	Transitions.push_back(Timer(shutter_time).Tween(&shutter_size.y, 0.0f).Finish([&](){shutter_on = !shutter_on; }));

	glm::ivec2 button_position = glm::ivec2(Application::width / 2, Application::height / 2) - glm::ivec2(32 * 2.7, 16 * 2);;
	SDL_Rect resume = {(int)button_position.x,(int)button_position.y,int(64.0 * 2.71),32 * 2 };

	resume_button = new Button(resume,"Resume", [&]() {Transitions.push_back(Timer(shutter_time).Tween(&shutter_size.y, 0.0f).Finish([&]() {shutter_on = !shutter_on; }));
	Application::g_logic->ui->PopThrough(resume_button); resume_state = false; }, ResourceManager::GetTexture("bidle")->id, ResourceManager::GetTexture("bhover")->id, ResourceManager::GetTexture("bclick")->id);

	SDL_Rect menu = { (int)button_position.x,(int)button_position.y+ 15 + 64,int(64.0 * 2.7),32 * 2 };

	menu_button = new Button(menu, "Menu", [&]() {Application::g_logic->ui->PopThrough(resume_button); delete resume_button; delete menu_button; StateManager::GetInstance()->ChangeState(new Menu); }, ResourceManager::GetTexture("bidle")->id, ResourceManager::GetTexture("bhover")->id, ResourceManager::GetTexture("bclick")->id);

	scale = 1.0f;
	resume_state = false;
	millisecond = minutes = second = hour = 0;
	return true;
}

bool Test::Exit()
{
	return true;
}

void Test::DrawBoard(glm::vec2 Offset)
{
	for (size_t y = 0; y < HEIGHT; y++)
	{
		for (size_t x = 0; x < WIDTH; x++)
		{
			Block block = match_table[(y * WIDTH) + x];

			if (block.blocked)
			{
				if (Cursor.x == x && Cursor.y == y && !(Select.x == x && Select.y == y))
				{
					BatchRenderer::DrawTexture(block.Position + Offset - glm::vec2(2.0f,2.0f), {block_size + 4.0f,block_size + 4.0f}, block.background_sprite);
					BatchRenderer::DrawTexture(block.Position + Offset + glm::vec2(14.0f, 15.0f) - glm::vec2(2.0f, 2.0f), { (block_size / 1.8) + 4.0f,(block_size / 1.8) + 4.0f }, block.foreground_sprite);
					continue;
				}
				if (Select.x == x && Select.y == y)
				{

					BatchRenderer::DrawTexture(block.Position + Offset, { block_size,block_size }, block.background_sprite, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
					BatchRenderer::DrawTexture(block.Position + Offset + glm::vec2(14.0f, 15.0f), { block_size / 1.8,block_size / 1.8 }, block.foreground_sprite, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
				}
				else
				{

					BatchRenderer::DrawTexture(block.Position + Offset, { block_size,block_size }, block.background_sprite);
					BatchRenderer::DrawTexture(block.Position + Offset + glm::vec2(14.0f, 15.0f), { block_size / 1.8,block_size / 1.8 }, block.foreground_sprite);

				}
			}
			
		}
	}

	for (size_t i = 0; i < Matches.size(); i++)
	{
		Block block = match_table[Matches[i]];
		BatchRenderer::DrawTexture(block.Position + Offset - glm::vec2(2.0f, 2.0f), { block_size + 4.0f,block_size + 4.0f }, select_background_frame);
		BatchRenderer::DrawTexture(block.Position + Offset + glm::vec2(14.0f, 15.0f) - glm::vec2(2.0f, 2.0f) - glm::vec2(3.0f,3.0f), {(block_size / 1.5f) + 4.0f,(block_size / 1.5f) + 4.0f}, block.foreground_sprite);
	}
}

bool Test::CheckMatch()
{
	 std::vector<size_t> tMatches;

	int Match = 0;
	BlockType type = Null;

	
		for (size_t y = 0; y < HEIGHT; y++)
		{
			for (size_t x = 0; x < WIDTH; x++)
			{
				if (match_table[(y * WIDTH) + x].blocked)
				{
					if (match_table[(y * WIDTH) + x].type == type)
					{
						Match++;
					}
					else
					{
						if (Match > 2)
						{
							for (size_t i = 1; i <= Match; i++)
							{
								tMatches.push_back((y * WIDTH) + (x - i));
							}
						}

						Match = 1;
						type = match_table[(y * WIDTH) + x].type;
					}
				}
				else
				{
					if (Match > 2)
					{
						for (size_t i = 1; i <= Match; i++)
						{
							tMatches.push_back((y * WIDTH) + (x - i));
						}
					}

					Match = 1;
					type = Null;
				}
			}

			if (Match > 2)
			{
				for (size_t i = 1; i <= Match; i++)
				{
					tMatches.push_back((y * WIDTH) + (WIDTH - i));
				}
			}

			Match = 1;
			type = Null;
		}



		for (size_t x = 0; x < WIDTH; x++)
		{
			for (size_t y = 0; y < HEIGHT; y++)
			{
				if (match_table[(y * WIDTH) + x].blocked)
				{
					if (match_table[(y * WIDTH) + x].type == type)
					{
						Match++;
					}
					else
					{
						if (Match > 2)
						{
							for (size_t i = 1; i <= Match; i++)
							{
								tMatches.push_back(((y - i) * WIDTH) + x);
							}
						}

						Match = 1;
						type = match_table[(y * WIDTH) + x].type;
					}
				}
				else
				{
					if (Match > 2)
					{
						for (size_t i = 1; i <= Match; i++)
						{
							tMatches.push_back(((y - i) * WIDTH) + x);
						}
					}

					Match = 1;
					type = Null;
				}
			}

			if (Match > 2)
			{
				for (size_t i = 1; i <= Match; i++)
				{
					tMatches.push_back(((HEIGHT - i) * WIDTH) + x);
				}
			}

			Match = 1;
			type = Null;
		}

		return !tMatches.empty();
}



Timer::Timer(double pDuration)
	:
	m_Duration(pDuration)
{
	m_Timer = 0.0;
	m_Done = false;
	m_Begin = 0.0f;
	m_End = 0.0f;
	m_ValuePtr = nullptr;

	m_Func = [&]() {};
}

void Timer::Update(double deltams)
{

	m_Timer = std::min(m_Duration, m_Timer + (1.0 / deltams));
	if(m_ValuePtr)
		*m_ValuePtr = m_Begin + ((m_End - m_Begin) * float(m_Timer / m_Duration));

	if (m_Timer == m_Duration)
	{
		m_Done = true;
		m_Func();

		if (!m_Chains.empty())
		{
			m_Duration = m_Chains.back().m_Duration;
			m_ValuePtr = m_Chains.back().m_ValuePtr;
			m_End = m_Chains.back().m_End;
			m_Timer = 0.0;
			m_Done = false;
			m_Begin = *m_ValuePtr;
			m_Func = m_Chains.back().m_Func;
			m_Chains.pop_back();
		}
	}
}

bool Timer::IsDone()
{
	return m_Done;
}

Timer Timer::Finish(std::function<void()> pFunc)
{
	m_Func = pFunc;

	return *this;
}

Timer Timer::Tween(float* pBegin, float pEnd)
{
	m_ValuePtr = pBegin;
	m_Begin = *pBegin;
	m_End = pEnd;

	return *this;
}

Timer Timer::Chain(std::vector<Timer> pChains)
{
	m_Chains = pChains;
	return *this;
}

void Menu::Update(double deltams)
{
	scale = float(((glm::sin((double)SDL_GetTicks()/1000)+1.0)/2.0)*0.5)+1.0f;

	timer->Update(deltams);

	if (timer->IsDone())
	{
		delete timer;
		timer = new Timer(*sound_loop);
	}
}

void Menu::Render()
{
	glm::vec2 size = Application::g_logic->f_Render->GetSize("Match", ResourceManager::GetFont("gluten"), scale);
	Application::g_logic->f_Render->RenderText(glm::vec2(Application::width/2.0f,100.0f) - glm::vec2(size.x/1.8f,0.0f), "Match", ResourceManager::GetFont("gluten"), {0.0f,0.0f,0.0f},scale);
}

void Menu::PollEvent(const SDL_Event& ev)
{
}

bool Menu::Enter()
{
	scale = 1.0f;
	glm::ivec2 button_position = glm::ivec2(Application::width / 2, Application::height / 2) - glm::ivec2(32*2, 16*2);

	Application::g_renderer->SetClearColor(glm::vec4(20.0f / 255.0f, 151.0f / 255.0f, 204.0f / 255.0f, 1.0f));
	
	SDL_Rect rect = { button_position.x,button_position.y,64*2,32*2 };
	button = new Button(rect,"Play", [&]() {Play(); }, ResourceManager::GetTexture("idle")->id, ResourceManager::GetTexture("hover")->id, ResourceManager::GetTexture("click")->id);

	SDL_Rect rect2 = { button_position.x,button_position.y+15+64,64 * 2,32 * 2 };

	exit_button = new Button(rect2,"Exit", [&]() {	Application::Quit();
	Application::quit = true; }, ResourceManager::GetTexture("idle")->id, ResourceManager::GetTexture("hover")->id, ResourceManager::GetTexture("click")->id);

	Application::g_logic->ui->Push(button);
	Application::g_logic->ui->Push(exit_button);

	sound_loop = new Timer(18.0);
	sound_loop->Finish([&]() { Audio::Play("retro_beat", { 0,0,0 }); });

	timer = new Timer(*sound_loop);

	Audio::Play("retro_beat", { 0,0,0 });

	return true;
}

bool Menu::Exit()
{
	Audio::Stop("retro_beat");
	delete timer;
	delete sound_loop;
	return true;
}

void Menu::Play()
{
	Application::g_logic->ui->PopThrough(button);
	delete button;
	delete exit_button;
	StateManager::GetInstance()->PopState();
	StateManager::GetInstance()->PushState(new Test);
}

Button::Button(SDL_Rect pRect, std::string pText, std::function<void()> pFunc, uint32_t idle, uint32_t hover, uint32_t click)
	:
	rect({pRect}), m_Func(pFunc), idleId(idle), hoverId(hover), clickId(click),m_Text(pText)
{
	idle = true;
	hover = false;
	click = false;
}


void Button::Draw()
{
	AddZone(rect.rect, [&]() {m_Func();
	click = true;
	hovered = false;
	idle = false;
	Audio::Play("click", { 0,0,0 });
		});

	BatchRenderer::BeginBatch();

	if(idle)
		BatchRenderer::DrawTexture(glm::vec2(rect.rect.x, rect.rect.y), glm::vec2(rect.rect.w, rect.rect.h), idleId);

	else if(hovered)
		BatchRenderer::DrawTexture(glm::vec2(rect.rect.x, rect.rect.y), glm::vec2(rect.rect.w, rect.rect.h), hoverId);

	else if(clickId)
		BatchRenderer::DrawTexture(glm::vec2(rect.rect.x, rect.rect.y), glm::vec2(rect.rect.w, rect.rect.h), clickId);


	BatchRenderer::EndBatch();
	BatchRenderer::Flush();

	glm::vec2 size = Application::g_logic->f_Render->GetSize(m_Text, ResourceManager::GetFont("thick_big"));
	Application::g_logic->f_Render->RenderText(glm::vec2(rect.rect.x + (rect.rect.w/2), rect.rect.y + (rect.rect.h/3.5)) - glm::vec2(size.x/1.8f,0.0f), m_Text.c_str(), ResourceManager::GetFont("thick_big"), {0.0f,0.0f,0.0f});
}



bool Button::Hover(int x, int y)
{
	if (rect.Contain(glm::vec2(x, y)))
	{
		hovered = true;
		idle = false;
		click = false;
		return true;
	}

	hovered = false;
	idle = true;
	click = false;

	return false;
}
