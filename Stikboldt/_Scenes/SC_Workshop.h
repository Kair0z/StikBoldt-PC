#pragma once
#include <GameScene.h>

class PF_WorkshopEnviroment;
class PF_RobotCharacter;

class SC_Workshop : public GameScene
{
public:
	SC_Workshop();
	virtual ~SC_Workshop() = default;

protected:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	PF_WorkshopEnviroment* m_pEnviroment;
	PF_RobotCharacter* m_pPlayer0;

	void InitPlayer();
	void InitEnviroment();

public:
	SC_Workshop(const SC_Workshop&) = delete;
	SC_Workshop(SC_Workshop&&) = delete;
	SC_Workshop& operator=(const SC_Workshop&) = delete;
	SC_Workshop& operator=(SC_Workshop&&) = delete;

};

