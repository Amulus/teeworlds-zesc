#ifndef GAME_SERVER_ENTITIES_LASER_H
#define GAME_SERVER_ENTITIES_LASER_H

#include <game/server/entity.h>

#include "../door.h"

class CDoorLaser : public CEntity
{
public:
	CDoorLaser(CGameWorld *pGameWorld, vec2 From, vec2 To, CDoor *pDoor);
	
	virtual void Snap(int SnappingClient);
	virtual void Tick();

protected:
	void HitCharacter(vec2 From, vec2 To);
	
private:
	CDoor *m_pDoor;
	vec2 m_From;
};

#endif
