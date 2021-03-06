#include "Components.h"
#include "Entity.h"
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include "Messages.h"
#include "Game.h"
#include "EntityFactory.h"
#include "DebugNew.h"
#include "Scenes.h"


#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayContainer.h>
#include <OgreFontManager.h>
#include <OgreOverlaySystem.h>

//Debug
#ifdef _DEBUG
#include <iostream>
#define new DEBUG_NEW
#endif





std::string compToString(ComponentType t, int &type){

	switch (t){
	case CMP_PASSIVE_HADES:
		type = 1;
		return "PASSIVE_HADES";
	case CMP_PASSIVE_HERMES:
		type = 1;
		return "PASSIVE_HERMES";
	case CMP_PASSIVE_SYN:
		type = 1;
		return "PASSIVE_SYN";
	case CMP_PASSIVE_ULL:
		type = 1;
		return "PASSIVE_ULL";
	case CMP_PASSIVE_VALI:
		type = 1;
		return "PASSIVE_VALI";
	case CMP_PASSIVE_VIDAR:
		type = 1;
		return "PASSIVE_VIDAR";
	case CMP_HERA_RUNE:
		type = 0;
		return "HERA_RUNE";
	case CMP_KHEPRI_BEETLE:
		type = 0;
		return "KHEPRI_BEETLE";
	case CMP_JONSU_MOON:
		type = 0;
		return "JONSU_MOON";
	case CMP_HERIS_MARK:
		type = 0;
		return "HERIS_MARK";
	case CMP_SHU_HEADDRESS:
		type = 0;
		return "SHU_HEADDRESS";
	default:
		break;
	}

};


/*-------------------------BASIC GAME COMPONENT------------------------------------*/
//Component base class, made for inheriting.
//It implements basic behaviours like gets, sets
//and message sending
#pragma region gameComponent
GameComponent::GameComponent(ComponentType componentType, Entity * father) : _componentType(componentType), pEnt(father)
{

}
GameComponent::~GameComponent(){

}
ComponentType GameComponent::getComponentType(){
	return _componentType;
}
bool GameComponent::getActive(){
	return _active;
}
void GameComponent::setActive(bool nw){
	_active = nw;
}
void GameComponent::sendMessage(Message * m){
	pEnt->getMessage(m);
}

#pragma endregion

/*-------------------------DEBUG COMPONENTS------------------------------------*/
//PRINTS A STRING WHEN RECEIVEING A STRING_MESSAGE
#pragma region stringComponent
CString::CString(Entity * fath) : GameComponent(CMP_STRING, fath), whatSay("HOLA, soy el componente basico"){

}
CString::~CString(){

}
void CString::getMessage(Message * m){
#ifdef _DEBUG
	//if (m->getType() == STRING_MSG)std::cout << "MESSAGE SAID: " << static_cast<stringMessage*>(m)->getText() << std::endl;

#endif
}
void CString::tick(float delta){
#ifdef _DEBUG
#endif
}

#pragma endregion

#pragma region messageSendComponent
CMessageSend::CMessageSend(Entity * father):GameComponent(CMP_MESSAGE_SEND, father) {
	i = 0;
}
CMessageSend::~CMessageSend() {
}
void CMessageSend::tick(float delta) {

}
void CMessageSend::getMessage(Message * m) {

}

#pragma endregion


/*-------------------------OGRE COMPONENTS------------------------------------*/
#pragma region RenderComponent
//Render Component class. Father to every
//other render component.

CRender::CRender(ComponentType t, Entity * father, Ogre::SceneManager * scnM)
	: GameComponent(t, father), pSceneMgr(scnM)
{
	pOgreSceneNode = pSceneMgr->getRootSceneNode()->createChildSceneNode();
	pChild = pOgreSceneNode->createChildSceneNode();
	lastDir = 3.0f;

}
CRender::~CRender(){
	pSceneMgr->destroySceneNode(pChild);
	pSceneMgr->destroySceneNode(pOgreSceneNode);
}

//Get Message general to every other render component child to this
void CRender::getMessage(Message *m) {
	switch (m->getType()) {
	case MSG_UPDATE_TRANSFORM:
		if (m->getEmmiter() == pEnt->getID()){
			//Message cast
			MUpdateTransform* msg = static_cast<MUpdateTransform *>(m);

			//We get the size of the colliderbox.
			float w = msg->getW();
			float h = msg->getH();

			//Where our node will rotate.
			Ogre::Vector3 parentPos = msg->GetPos();

			//Where our mesh is relative to the parent.
			//The real pos of the object is the parent pos + this variable, _ogrepos.
			float angleRad = msg->getRotation();

			if (angleRad != -1){
				_ogrepos.x = w / 2;
				_ogrepos.y = 0;
				_ogrepos.z = 0;

				//Move the parent to the collider location of rotation.
				pOgreSceneNode->setPosition(parentPos);
				//Move the child to the real pos of the collider.
				pChild->setPosition(_ogrepos);

				//Rotate the parent node the same degree as the collider.
				float grades = (angleRad * 180) / 3.14159265359;
				pOgreSceneNode->setOrientation(Ogre::Quaternion(Ogre::Degree(grades), Ogre::Vector3(0, 0, 1)));
			}
			else{

				_ogrepos.x = w / 2;
				_ogrepos.y = 0;
				_ogrepos.z = 0;

				/*float actualX = pOgreSceneNode->getPosition().x;
				float dir = parentPos.x - actualX;
				float angle = pChild->getOrientation().getYaw().valueDegrees();

				if (dir != 0){
					if (dir < 0){
						pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-angle), Ogre::Vector3(0, 1, 0)));
						pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3(0, 1, 0)));
					}
					else{
						pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-angle), Ogre::Vector3(0, 1, 0)));
						pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0, 1, 0)));

					}
				}*/


				//Move the parent to the collider location of rotation.
				pOgreSceneNode->setPosition(parentPos);
				//Move the child to the real pos of the collider.
				pChild->setPosition(_ogrepos);


			}


		}
		break;
	default:
		break;
	}
}

#pragma endregion
#pragma region meshRenderComponent
//Mesh Render component.
//Takes a string with the name of the mesh to render
//and renders it.

CMeshRender::CMeshRender(Ogre::Vector3 pos, std::string meshName, Entity * father, Ogre::SceneManager * scnM, Ogre::Vector3 scale, Ogre::Vector3 rotation) :CRender(CMP_MESH_RENDER, father, scnM) {
	pOgreEnt = pSceneMgr->createEntity(father->getID(),meshName);
	pOgreSceneNode->setPosition(pos);

	pChild->attachObject(pOgreEnt);
	pChild->scale(scale);
	//pChild->showBoundingBox(true);
	//pOgreEnt->setCastShadows(true);

	pOgreSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(rotation.x), Ogre::Vector3(1, 0, 0)));
	pOgreSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(rotation.y), Ogre::Vector3(0, 1, 0)));
	pOgreSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(rotation.z), Ogre::Vector3(0, 0, 1)));

	invisActive = false;


}
CMeshRender::~CMeshRender() {
	pChild->detachObject(pOgreEnt);
	pSceneMgr->destroyEntity(pOgreEnt);
}
void CMeshRender::tick(float delta) {


}
void CMeshRender::getMessage(Message * m) {
	CRender::getMessage(m);
	float angle;
	float dir;
	MPlayerShot* mPShot;
	//TO DO: IMPLEMENT MESSAGE RECEIVEING TO MOVE.
	switch (m->getType())
	{
	case MSG_MOD_INVS:
		invisActive = true;
		break;

	case MSG_PLAYER_JUMP:
		if (invisActive){
			pOgreEnt->setVisible(false);
		}
		break;

	case MSG_COLLISION_TERRAIN:
		if (invisActive){
			pOgreEnt->setVisible(true);
		}
		break;
	case MSG_PLAYER_SHOT:
		break;
	case MSG_PASSMOD_DES:
		invisActive = false;
		break;
	case MSG_ACT_RENDERGODMENU:
		pOgreEnt->setVisible(true);
		break;
	case MSG_DES_RENDERGODMENU:
		pOgreEnt->setVisible(false);
		break;
	default:
		break;
	}
}

Ogre::Vector3 CMeshRender::getSize(){

	Ogre::Vector3 v;

	//Ogre::AxisAlignedBox box = pSceneMgr->getSceneNode(pChild->getName())->_getWorldAABB();

	Ogre::AxisAlignedBox box = pOgreEnt->getWorldBoundingBox(true);

	v = box.getSize();

	return v;
}


#pragma endregion

#pragma region Animation Component
CAnimation::CAnimation(Entity * father, Ogre::SceneManager * scnM, Ogre::SceneNode* child) : GameComponent(CMP_ANIMATION, father){
	pOgreEnt = scnM->getEntity(father->getID());
	pChild = child;
	isShooting = false;
	Ogre::Skeleton* sk = pOgreEnt->getSkeleton();

	idleBot = pOgreEnt->getAnimationState("idleBot");
	moveBot = pOgreEnt->getAnimationState("moveBot");
	jumpBot = pOgreEnt->getAnimationState("jumpBot");
	airBot  = pOgreEnt->getAnimationState("airBot");

	idleTop = pOgreEnt->getAnimationState("idleTop");
	moveTop = pOgreEnt->getAnimationState("moveTop");
	//jumpTop = pOgreEnt->getAnimationState("Jump Top");
	airTop  = pOgreEnt->getAnimationState("airTop");
	//chargeTop = pOgreEnt->getAnimationState("chargeTop");
	shootTop = pOgreEnt->getAnimationState("shootTop");
	start = pOgreEnt->getAnimationState("start");

	start->setLoop(false);
	idleBot->setLoop(true);
	moveBot->setLoop(true);
	jumpBot->setLoop(false);
	airBot->setLoop(true);

	idleTop->setLoop(true);
	moveTop->setLoop(true);
	//jumpTop->setLoop(false);
	airTop->setLoop(true);
	//chargeTop->setLoop(false);
	shootTop->setLoop(false);
	//Initial rotation
	pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3(0, 1, 0)));


	currentBot = idleBot;
	currentBot->setEnabled(true);
	currentTop = idleTop;
	currentTop->setEnabled(true);
	nextBot = nullptr;
	nextTop = nullptr;
	starting = false;

}
CAnimation::~CAnimation(){
}


void CAnimation::tick(float delta){

	//std::cout << currentBot->getAnimationName() << std::endl;
	//std::cout << currentTop->getAnimationName() << std::endl;

	/*if (nextBot != nullptr){
		currentBot = nextBot;
		nextBot = nullptr;
	}
	*/

	if (currentTop->getLength() == currentTop->getTimePosition()){
		currentTop->setEnabled(false);
		if (nextTop != nullptr){
			nextTop->setTimePosition(0);
			nextTop->setEnabled(true);
			currentTop = nextTop;
		}
		isShooting = false;
		starting = false;
	}

	if (currentBot->getLength() == currentBot->getTimePosition()){
		currentBot->setEnabled(false);
		if (nextBot != nullptr){
			nextBot->setTimePosition(0);
			nextBot->setEnabled(true);
			currentBot = nextBot;
		}

	}


	if (starting){
		currentTop->addTime(delta);
	}
	else{
		currentTop->addTime(delta * 3);
		currentBot->addTime(delta*3);
	}

}
void CAnimation::getMessage(Message * m){
	MPlayerMoveX* mMoveX;
	float angle;
	float dir;
	switch (m->getType())
	{
	case MSG_PLAYER_MOVE_X:
		mMoveX = static_cast<MPlayerMoveX*>(m);


		if (!isShooting){
			dir = mMoveX->GetValue();
			angle = pChild->getOrientation().getYaw().valueDegrees();

			if (dir != 0){
				if (dir < 0){
					pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-angle), Ogre::Vector3(0, 1, 0)));
					pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3(0, 1, 0)));
				}
				else{
					pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-angle), Ogre::Vector3(0, 1, 0)));
					pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0, 1, 0)));

				}
			}
		}



		if (mMoveX->GetValue() != 0){
			if (_air)
				changeAnim(airBot, airTop,false,false);
			else
				changeAnim(moveBot, moveTop, true,false);

		}
		else{
			changeAnim(idleBot, idleTop, true,false);
		}
		break;

	case MSG_RIGIDBODY_JUMP:
		changeAnim(jumpBot, airTop, false,false);
		 _air = true;
		break;

	case MSG_COLLISION_TERRAIN:
		_air = false;
		break;

	case MSG_SHOT:

		isShooting = true;
		angle = pChild->getOrientation().getYaw().valueDegrees();

		dir = static_cast<MPlayerShot*>(m)->getXValue();
		if (dir != 0){
			if (dir < 0){
				pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-angle), Ogre::Vector3(0, 1, 0)));
				pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3(0, 1, 0)));
			}
			else{
				pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(-angle), Ogre::Vector3(0, 1, 0)));
				pChild->setOrientation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0, 1, 0)));

			}
		}
		changeAnim(currentBot, shootTop, false,true);

		break;
	default:
		break;
	}
}
void CAnimation::changeAnim(Ogre::AnimationState* nextB, Ogre::AnimationState* nextT, bool loop, bool shoot){

	if (nextT != nullptr){
		if (currentTop->getLoop()){
			if (nextT != currentTop){
				currentTop->setEnabled(false);
				nextT->setTimePosition(0);
				nextT->setEnabled(true);
				nextT->setLoop(loop);
				currentTop = nextT;
			}
		}
		else{

			if (shoot){
				if (nextT != currentTop){
					currentTop->setEnabled(false);
					nextT->setTimePosition(0);
					nextT->setEnabled(true);
					nextT->setLoop(loop);
					currentTop = nextT;
				}
			}
			else{
				nextTop = nextT;
				nextTop->setLoop(loop);

			}

		}
	}

	if (nextB != nullptr){

		if (currentBot->getLoop()){
			if (nextB != currentBot){
				currentBot->setEnabled(false);
				nextB->setTimePosition(0);
				nextB->setEnabled(true);
				nextB->setLoop(loop);
				currentBot = nextB;
			}
		}
		else{

			nextBot = nextB;
			nextBot->setLoop(loop);

		}
	}





}
#pragma endregion



#pragma region Skyplane Render Component
CSkyPlaneRender::CSkyPlaneRender(Entity * father, Ogre::SceneManager * scnM, float scale, std::string materialName, Ogre::Vector3 pos, Ogre::Viewport* vp) :CRender(CMP_SKYPLANE_RENDER, father, scnM){

	plane = Ogre::MeshManager::getSingleton().createPlane(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::Plane(Ogre::Vector3::UNIT_Z, 0),
		96, 54, 100, 100, false, 1, 1.0, 1.0);
	entity = scnM->createEntity("plano" + materialName + pEnt->getScene()->getId(), materialName);

	entity->setMaterialName(materialName);
	pOgreSceneNode->attachObject(entity);

	pOgreSceneNode->setPosition(pos);
	//pOgreSceneNode->setScale({ scale, scale, scale });

}
CSkyPlaneRender::~CSkyPlaneRender(){

	Ogre::MeshManager::getSingleton().remove(plane);

	pOgreSceneNode->detachObject(entity);
	pSceneMgr->destroyEntity(entity);
}


void CSkyPlaneRender::tick(float delta){}
void CSkyPlaneRender::getMessage(Message * m){}


#pragma endregion
/*------------------------- RENDER COMPONENTS------------------------------------*/
#pragma region particleRenderComponent

//PARTICLES
CParticleRender::CParticleRender(Ogre::Vector3 pos, std::string id,std::string particleSystem, Entity * father, Ogre::SceneManager * scnM, Ogre::Vector3 scale, Ogre::Vector3 rotation) :CRender(CMP_PARTICLE_RENDER , father, scnM) {


	_particleSystem = scnM->createParticleSystem(id, particleSystem);
	pChild->attachObject(_particleSystem);

	pOgreSceneNode->setPosition(pos);
	pChild->scale(scale);

	pOgreSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(rotation.x), Ogre::Vector3(1, 0, 0)));
	pOgreSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(rotation.y), Ogre::Vector3(0, 1, 0)));
	pOgreSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(rotation.z), Ogre::Vector3(0, 0, 1)));

	_particleSystem->setEmitting(true);


}
CParticleRender::~CParticleRender() {
	pChild->detachObject(_particleSystem);
	pSceneMgr->destroyParticleSystem(_particleSystem);
}
void CParticleRender::tick(float delta) {



}
void CParticleRender::getMessage(Message * m) {
	CRender::getMessage(m);

}
#pragma endregion


#pragma region ribbonTrailRenderComponent

// RIBBON TRAIL
CRibbonTrailRender::CRibbonTrailRender(Ogre::Vector3 pos, std::string id, std::string particleSystem, Entity * father, Ogre::SceneManager * scnM, Ogre::Vector3 scale, Ogre::Vector3 rotation) :CRender(CMP_PARTICLE_RENDER, father, scnM) {

	trail = scnM->createRibbonTrail("trail");
	trail->setMaxChainElements(30);
	trail->setInitialColour(0, 0.0f, 0.0f, 1.0f, 1.0f);
	trail->setColourChange(0, 0.0f, 0.0f, 0.1f, 0.2f);
	trail->setInitialWidth(0, 0.3);
	trail->addNode(pChild);
	scnM->getRootSceneNode()->attachObject(trail);
}
CRibbonTrailRender::~CRibbonTrailRender() {
	pChild->detachObject(trail);
}
void CRibbonTrailRender::tick(float delta) {



}
void CRibbonTrailRender::getMessage(Message * m) {
	CRender::getMessage(m);

}
#pragma endregion


/*------------------------- CAMERA COMPONENTS------------------------------------*/
#pragma region Camera Component
CCamera::CCamera(Entity * father, Ogre::SceneManager * scnMgr, Ogre::Viewport * vp, std::string camName, Ogre::Vector3 pos, Ogre::Vector3 lookAt, int clipDistance)
	: GameComponent(CMP_CAMERA, father), _scnMgr(scnMgr), _camName(camName), _vp(vp), _pos(pos), _lookAt(lookAt), pCam(0)
{
	pCam = _scnMgr->createCamera(_camName);
	Ogre::RenderWindow* w = Game::getInstance()->getRenderWindow();
	vp = w->addViewport(pCam);


	pCam->setPosition(_pos);
	pCam->lookAt(_lookAt);
	pCam->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) /
		Ogre::Real(vp->getActualHeight()));

	pCam->setNearClipDistance(clipDistance);

	vp->setBackgroundColour(Ogre::ColourValue(0.2, 0.5, 0.8));

	//borrar
	_vp = vp;


}
CCamera::~CCamera() {
	delete pCam;
}
void CCamera::tick(float delta) {
	if (_lookAt != _lastLookAt) {
		pCam->lookAt(_lookAt);
		_lastLookAt = _lookAt;
	}
	if (_pos != _lastPos) {
		pCam->setPosition(_pos);
		_lastPos = _pos;
	}

}
void CCamera::getMessage(Message * m) {

}
#pragma endregion



#pragma region Action Camera Component
CActionCamera::CActionCamera(Entity * father, Ogre::SceneManager * scnMgr, Ogre::Viewport * vp, float xBoundary, float yBoundary, float minZ, float maxZ):
	CCamera(father, scnMgr, vp, "MainCamera", Ogre::Vector3(0,0,100), Ogre::Vector3(0,0,0), 5),
	smooth(40.0), MAXZ(maxZ), MINZ(minZ), BOUNDARY_X(xBoundary), BOUNDARY_Y(yBoundary) {
	_pj1 = Ogre::Vector3(20, 20, 0);
	_pj2 = Ogre::Vector3(-20, 20, 0);

	//Inicializating values for the camera
	_lastPos = Ogre::Vector3(0, 0, 100);
	_pos = Ogre::Vector3(0, 0, 100);

	//We start looking at the center of the 2 players
	_lookAt = (_pj1 + _pj2) / 2.0;

	//And we calculate the zRatio.
	zRatio = (_pj1.distance(_pj2) / _pos.z);
}

CActionCamera::~CActionCamera() {

}

//Function that calculates if a certain point is out of the defined boundaries for the camera
bool CActionCamera::outOfBoundaries(const Ogre::Vector3 &pos) {
	//Boundaries in each axis


	if ((pos.x > BOUNDARY_X || pos.x < -BOUNDARY_X) || (pos.y > BOUNDARY_Y || pos.y < -BOUNDARY_Y))return true;
	else return false;
}
void CActionCamera::getMessage(Message * m) {

	if (m->getType() == MSG_CAMERA_FOLLOW && static_cast<MCameraFollow *>(m)->getEmmiter() == "Player_0") {
		//We check if the player is inside the boundaries of the camera. If so, we put its camera position to 0.0.0
		if (outOfBoundaries(static_cast<MCameraFollow *>(m)->GetPos()))
			_pj1 = Ogre::Vector3::ZERO;
		//if it has moved from its previous position, we update its position
		else
			_pj1 = static_cast<MCameraFollow * >(m)->GetPos();

	}
	else if (m->getType() == MSG_CAMERA_FOLLOW && static_cast<MCameraFollow *>(m)->getEmmiter() == "Player_1") {
		if (outOfBoundaries(static_cast<MCameraFollow *>(m)->GetPos()))
			_pj2 = Ogre::Vector3::ZERO;
		else
			_pj2 = static_cast<MCameraFollow * >(m)->GetPos();
	}
	else return;

	//We calculate the midpoint between the 2 players
	_newPos = (_pj1 + _pj2) / 2.0;

	//we want the camera to be always a bit higher than the players
	_newPos.y += 5;

	//Now we want to know the camera new Z position
	float camz = (_pj1.distance(_pj2) / zRatio);


	//We dont want the camera to go too far away or too close
	if (camz >= MAXZ)_newPos.z = MAXZ;
	else if (camz < MINZ)_newPos.z = MINZ;
	else _newPos.z = camz;

	//Now we want to make it smooth, for that we calculate the director vector of the line.
	//And we divide it by the smooth factor we declared at initialization
	Ogre::Vector3 dir = (_newPos - _pos) / smooth;


	_pos += dir;
	_lookAt = _pos;
	_lookAt.z = 0;
	_lookAt.y += 10;


}

void CActionCamera::tick(float delta) {
	CCamera::tick(delta);

}

#pragma endregion
/*-------------------------BOX2D COMPONENTS------------------------------------*/

#pragma region RigidBodyComponent
//Rigid Body component.
//Gives an entity a rigid body to simulate physics
CRigidBody::CRigidBody(Entity * father, b2World * world, Ogre::Vector3 posInPixels, float heightInPixels, float weightInPixels, float angle, RigidBodyType rbType, ShapeType shType, FilterMask myCategory, int controllerId)
: _rbHeight(heightInPixels / PPM), _rbWeight(weightInPixels / PPM), _myWorld(world), GameComponent(CMP_PHYSICS,father) {

	//soundW = Game::getInstance()->getSoundEngine()->addSoundSourceFromFile("../Media/sounds/Movement/paso1.wav");

	//soundJ = Game::getInstance()->getSoundEngine()->addSoundSourceFromFile("../Media/sounds/Movement/SaltoAire.wav");
	//Sets the pos attached to the render.
	_pos.x = posInPixels.x / PPM;
	_pos.y = posInPixels.y / PPM;

	//Initial velocity 0.
	_vel.x = _vel.y = 0;

	//Body definition.
	_bodyDef.position.Set(_pos.x, _pos.y);



	if (myCategory == MASK_PLAYER)
		_bodyDef.fixedRotation = true;
	if (myCategory == MASK_BULLET_0 || myCategory == MASK_BULLET_1){
		_bodyDef.bullet = true;
		//_bodyDef.fixedRotation = true;
		_bodyDef.gravityScale = 0.0f;
	}

	/*_bodyDef.linearDamping = 5.0f;
	_bodyDef.angularDamping = 0.0f;*/

	switch (rbType)
	{
		case RB_DYNAMIC:
			_bodyDef.type = b2_dynamicBody;
			break;
		case RB_STATIC:
			_bodyDef.type = b2_staticBody;
			break;
		case RB_KINEMATIC:
			_bodyDef.type = b2_kinematicBody;
			break;
		default:
			_bodyDef.type = b2_staticBody;
			break;
	}
	//Body creation.
	_body = _myWorld->CreateBody(&_bodyDef);

	float radians = (3.14159265359f * angle) / 180.0f;

	_body->SetTransform(_pos,radians);

	//Set the body data pointer to entity
	_body->SetUserData(pEnt);

	//Fixture Definition.
	_fixtureDef.density = 9.0f / 3.0f;
	//_fixtureDef.restitution = 0.0f;
	_fixtureDef.friction = 0.0f;

	switch (myCategory)
	{
		case MASK_STATIC_TERRAIN:
			_fixtureDef.filter.categoryBits = MASK_STATIC_TERRAIN;
			_fixtureDef.filter.maskBits = MASK_BULLET_0 | MASK_BULLET_1 | MASK_DINAMIC_TERRAIN | MASK_LEGS_0 | MASK_LEGS_1| MASK_HEAD_1 | MASK_HEAD_0 | MASK_CHEST_0 | MASK_CHEST_1 | MASK_FOOT_SENSOR;
			break;
		case MASK_DINAMIC_TERRAIN:
			_fixtureDef.filter.categoryBits = MASK_DINAMIC_TERRAIN;
			_fixtureDef.filter.maskBits = MASK_BULLET_0 | MASK_BULLET_1 | MASK_STATIC_TERRAIN | MASK_LEGS_0 | MASK_LEGS_1 | MASK_HEAD_1 | MASK_HEAD_0 | MASK_CHEST_0 | MASK_CHEST_1 | MASK_FOOT_SENSOR;
			break;
		case MASK_BULLET_0:
			_fixtureDef.filter.categoryBits = MASK_BULLET_0;
			_fixtureDef.filter.maskBits = MASK_LEGS_1 | MASK_CHEST_1 | MASK_HEAD_1 | MASK_DINAMIC_TERRAIN | MASK_STATIC_TERRAIN | MASK_DEATHZONE;
			break;
		case MASK_BULLET_1:
			_fixtureDef.filter.categoryBits = MASK_BULLET_1;
			_fixtureDef.filter.maskBits = MASK_LEGS_0 | MASK_CHEST_0 | MASK_HEAD_0 | MASK_DINAMIC_TERRAIN | MASK_STATIC_TERRAIN | MASK_DEATHZONE;
			break;
		case MASK_DEATHZONE:
			_fixtureDef.filter.categoryBits = MASK_DEATHZONE;
			_fixtureDef.filter.maskBits = MASK_LEGS_0 | MASK_CHEST_0 | MASK_HEAD_0 | MASK_BULLET_0 | MASK_LEGS_1 | MASK_CHEST_1 | MASK_HEAD_1 | MASK_BULLET_1;
			break;
		default:
			break;
	}

	float playerSize = _rbHeight / 3;
	float legsSize = _rbWeight / 1.5f;

	b2PolygonShape _pShape;

	b2CircleShape _cShape;

	//Shape creation.
	switch (shType)
	{
	case SH_CIRCLE:
		_cShape.m_p.Set(0, 0);
		_cShape.m_radius = _rbWeight;
		_fixtureDef.shape = &_cShape;
		//Fixture creation.
		_fixture = _body->CreateFixture(&_fixtureDef);
		break;
	case SH_POLYGON:

		_pShape.SetAsBox(_rbWeight / 2, _rbHeight / 2, { _rbWeight / 2, _rbHeight / 2 }, 0);
		_fixtureDef.shape = &_pShape;

		//Fixture creation.
		_fixture = _body->CreateFixture(&_fixtureDef);
		break;
	case SH_PLAYER:

		_pShape.SetAsBox(_rbWeight / 2, playerSize / 2, { _rbWeight / 2, _rbHeight / 2 }, 0);
		_fixtureDef.shape = &_pShape;

		if (controllerId == 1){
			_fixtureDef.filter.categoryBits = MASK_CHEST_1;
			_fixtureDef.filter.maskBits = MASK_STATIC_TERRAIN | MASK_DEATHZONE | MASK_DINAMIC_TERRAIN | MASK_CHEST_0 | MASK_HEAD_0 | MASK_LEGS_0 | MASK_BULLET_0;

		}
		else{

			_fixtureDef.filter.categoryBits = MASK_CHEST_0;
			_fixtureDef.filter.maskBits = MASK_STATIC_TERRAIN | MASK_DEATHZONE | MASK_DINAMIC_TERRAIN | MASK_HEAD_1 | MASK_CHEST_1 | MASK_LEGS_1| MASK_BULLET_1;

		}
		_body->CreateFixture(&_fixtureDef);

		_pShape.SetAsBox(_rbWeight / 2, playerSize / 2, { _rbWeight / 2, _rbHeight / 2 + playerSize }, 0);
		_fixtureDef.shape = &_pShape;
		if (controllerId == 1){
			_fixtureDef.filter.categoryBits = MASK_HEAD_1;
			_fixtureDef.filter.maskBits = MASK_STATIC_TERRAIN | MASK_DEATHZONE | MASK_DINAMIC_TERRAIN | MASK_CHEST_0 | MASK_HEAD_0 | MASK_LEGS_0 | MASK_BULLET_0;

		}
		else{

			_fixtureDef.filter.categoryBits = MASK_HEAD_0;
			_fixtureDef.filter.maskBits = MASK_STATIC_TERRAIN | MASK_DEATHZONE | MASK_DINAMIC_TERRAIN | MASK_HEAD_1 | MASK_CHEST_1 | MASK_LEGS_1 | MASK_BULLET_1;

		}
		_body->CreateFixture(&_fixtureDef);

		_pShape.SetAsBox(_rbWeight / 2, playerSize / 2, { _rbWeight / 2, _rbHeight / 2 - playerSize }, 0);
		_fixtureDef.shape = &_pShape;
		if (controllerId == 1){
			_fixtureDef.filter.categoryBits = MASK_LEGS_1;
			_fixtureDef.filter.maskBits = MASK_STATIC_TERRAIN | MASK_DEATHZONE | MASK_DINAMIC_TERRAIN | MASK_CHEST_0 | MASK_HEAD_0 | MASK_LEGS_0 | MASK_BULLET_0;

		}
		else{

			_fixtureDef.filter.categoryBits = MASK_LEGS_0;
			_fixtureDef.filter.maskBits = MASK_STATIC_TERRAIN | MASK_DEATHZONE | MASK_DINAMIC_TERRAIN | MASK_HEAD_1 | MASK_CHEST_1 | MASK_LEGS_1 | MASK_BULLET_1;

		}
		_fixture = _body->CreateFixture(&_fixtureDef);

		//Foot sensor
		_pShape.SetAsBox(legsSize / 2, playerSize / 2, { _rbWeight / 2, 0 }, 0);
		//_fixtureDef.isSensor = true;
		_fixtureDef.shape = &_pShape;
		_fixtureDef.density = 1.0f;
		_fixtureDef.filter.categoryBits = MASK_FOOT_SENSOR;
		_fixtureDef.filter.maskBits = MASK_DINAMIC_TERRAIN | MASK_STATIC_TERRAIN;
		_fixture = _body->CreateFixture(&_fixtureDef);
		_fixture->SetSensor(true);
		break;
	default:
		break;
	}





}
CRigidBody::~CRigidBody() {
	pEnt->getScene()->addBodyToDelete(_body);

}
void CRigidBody::tick(float delta) {



	//Send the message to the entity.
	//Transformation from physics world to ogre world.

	if (_body->IsFixedRotation()){
		MUpdateTransform * m = new MUpdateTransform(Ogre::Vector3((_body->GetPosition().x )* PPM , _body->GetPosition().y * PPM, 0), -1,_rbHeight * PPM, _rbWeight * PPM, pEnt->getID());
		pEnt->getMessage(m);
	}
	else{

		MUpdateTransform * m = new MUpdateTransform(Ogre::Vector3((_body->GetPosition().x)* PPM, _body->GetPosition().y * PPM, 0), _body->GetAngle(), _rbHeight * PPM, _rbWeight * PPM, pEnt->getID());
		pEnt->getMessage(m);

	}

	//if (pEnt->getID() == "Player_0";
	//std::cout << _body->GetPosition().x * PPM << std::endl;


}

void CRigidBody::getMessage(Message * m) {


	MRigidbodyMoveX* mMoveX;
	float velX;
	MRigidbodyMoveY* mMoveY;
	float velY;
	MRigidbodyJump* mJump;
	float jForce;
	MDash* mDash;
	bool playingW = false;
	irrklang::ISound* s = nullptr;

	switch (m->getType()){
		case MSG_RIGIDBODY_MOVE_X:
			mMoveX = static_cast<MRigidbodyMoveX*>(m);
			velX = mMoveX->getXValue();




			_body->SetLinearVelocity(b2Vec2(velX, _body->GetLinearVelocity().y));
			break;
		case MSG_RIGIDBODY_MOVE_Y:
			mMoveY = static_cast<MRigidbodyMoveY*>(m);
			velY = mMoveY->getYValue();
			_body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, velY));
			break;
		case MSG_RIGIDBODY_JUMP:
			mJump = static_cast<MRigidbodyJump*>(m);
			jForce = mJump->getForce();



			Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/Movement/SaltoAire.wav");



			//std::cout << jForce << std::endl;
			_body->ApplyLinearImpulseToCenter(b2Vec2(0, jForce), true);
			break;
		case MSG_DASH:
			mDash = static_cast<MDash*>(m);
			_body->ApplyLinearImpulseToCenter(b2Vec2(mDash->getDashValue()->x, mDash->getDashValue()->y), true);
			break;
		default:
			break;
	}
}
#pragma endregion

#pragma region CollisionHandler
CPlayerCollisionHandler::CPlayerCollisionHandler(Entity* father) :GameComponent(CMP_PLAYER_CH, father) {
}
CPlayerCollisionHandler::~CPlayerCollisionHandler(){
}

void CPlayerCollisionHandler::tick(float delta){}
void CPlayerCollisionHandler::getMessage(Message * m){


	if (m->getType() == MSG_COLLISION){

		MCollisionBegin* mColBegin = static_cast<MCollisionBegin*>(m);
		_myMask = mColBegin->GetMyCategory();
		//std::cout << mColBegin->GetContactMask() << std::endl;
		switch (mColBegin->GetContactMask()){
			case MASK_STATIC_TERRAIN:
				if (_myMask == MASK_FOOT_SENSOR)
					pEnt->getMessage(new MCollisionTerrain(pEnt->getID()));
				break;
			case MASK_DEATHZONE:
				pEnt->getMessage(new MDie(pEnt->getID()));
				break;
			default:
				break;
		}
	}
	else if (m->getType() == MSG_BULLET_HIT){
		MBulletHit* mBH = static_cast<MBulletHit*>(m);
		if (mBH->getTargetMask() == MASK_CHEST_0 || mBH->getTargetMask() == MASK_CHEST_1)
			pEnt->getMessage(new MDamage(mBH->getDamage(), pEnt->getID()));
		else
			pEnt->getMessage(new MDamageArmature(mBH->getDamage(), mBH->getTargetMask(), pEnt->getID()));
	}
}
#pragma endregion

//Player Controller Component
#pragma region PlayerControllerComponent
CPlayerController::CPlayerController(Entity* f, int i): GameComponent(CMP_PLAYER_CONTROLLER, f), _id(i){

}

CPlayerController::~CPlayerController(){}

void CPlayerController::tick(float delta){
}

void CPlayerController::getMessage(Message* m){
	//If the msg type is CInputState, read the input and process it
	if (m->getType() == MSG_INPUT_STATE){
		MInputState* inputM = static_cast<MInputState*>(m);
		if (inputM->getId() == _id){

			ControllerInputState cState = inputM->getCInputState();

			if (cState.Right_Shoulder == BTT_PRESSED){
				MJump* m = new MJump( pEnt->getID());
				pEnt->getMessage(m);
				//Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/Pruebo.ogg");
			}


			float xValue = cState.Axis_RightX;
			float yValue = cState.Axis_RightY;
			//Check joystick rotation, to control the bullet spawn
			if (!(xValue == 0.0f && yValue == 0.0f )){
				MPlayerShot* m = new MPlayerShot(xValue, yValue, pEnt->getID());
				pEnt->getMessage(m);
			}
			if (cState.Axis_LeftX > AXIS_DEADZONE){
				MPlayerMoveX* m = new MPlayerMoveX(cState.Axis_LeftX, _id, pEnt->getID());
				pEnt->getMessage(m);
			}
			else if (cState.Axis_LeftX < -AXIS_DEADZONE){
				MPlayerMoveX* m = new MPlayerMoveX(cState.Axis_LeftX, _id, pEnt->getID());
				pEnt->getMessage(m);
			}
			else if (cState.Axis_LeftX <= AXIS_DEADZONE && cState.Axis_LeftX >= -AXIS_DEADZONE){
				MPlayerMoveX* m = new MPlayerMoveX(0, _id, pEnt->getID());
				pEnt->getMessage(m);
			}
		}
	}

}

#pragma endregion

//Life Component
#pragma region Life Component
CLife::CLife(Entity* father, float iniLife) :GameComponent(CMP_LIFE, father), _maxLife(iniLife), _currentLife(iniLife) {


}
CLife::~CLife(){}

void CLife::tick(float delta){



#ifdef DEBUG

	//std::cout << _currentLife << std::endl;
#endif //  DEBUG

}
void CLife::getMessage(Message* m){

	switch (m->getType()){
	case MSG_DAMAGE:
		if (_currentLife > 0.0f){
			_currentLife -= static_cast<MDamage*>(m)->getDamage();
			pEnt->getMessage(new MLifeState(pEnt->getID(), _currentLife));
			if (_currentLife <= 0.0f){
				pEnt->getMessage(new MDie(pEnt->getID()));
			}
		}
		break;
	default:
		break;

	}
}
#pragma endregion

//Move Component
#pragma region Player Move Component
CPlayerMove::CPlayerMove(Entity* father, float vel) :GameComponent(CMP_MOVEMENT_SPEED, father), _maxSpeed(MAX_SPEED), _moveVel(vel), _auxVelReset(vel){}
CPlayerMove::~CPlayerMove(){}

void CPlayerMove::tick(float delta){}
void CPlayerMove::getMessage(Message* m)
{
	if (m->getType() == MSG_PLAYER_MOVE_X){
		//transformarlo
		float value = static_cast<MPlayerMoveX*>(m)->GetValue();
		//value = value / 40;

		if (value > _moveVel)
			value = _moveVel;
		else if (value < - _moveVel)
			value = - _moveVel;

		pEnt->getMessage(new MRigidbodyMoveX(value, pEnt->getID()));
	}

	if (m->getType() == MSG_MOD_VEL){
		float mModVelValue = static_cast<MModVel*>(m)->getValue();
		_moveVel = _moveVel + (_moveVel * mModVelValue / 100.0f);
		if (_moveVel > _maxSpeed){
			_moveVel = _maxSpeed;
		}
	}

	if (m->getType() == MSG_MOD_VEL_JUMP){
		float mModVelValue = static_cast<MModVelAndJump*>(m)->getVelValue();
		_moveVel = _moveVel + (_moveVel * mModVelValue / 100.0f);
		if (_moveVel > _maxSpeed){
			_moveVel = _maxSpeed;
		}
	}

	if (m->getType() == MSG_PASSMOD_DES){
		resetVel();
	}

	if (m->getType() == MSG_ACTIVEMOD_RES){
		resetVel();
	}

}
#pragma endregion

//Jump Component
#pragma region Player Jump Component

CPlayerJump::CPlayerJump(Entity* father, float startForce) :GameComponent(CMP_JUMP, father), _maxForce(MAX_JUMP_DISTANCE), _jumpForce(startForce), _maxJumps(2), _nJumps(0), _auxJumpReset(startForce)
{
	_timeCounter = 0.0f;
	_lastTimeJump = 0.0f;
	_jumpRate = 170.0f;

}

CPlayerJump::~CPlayerJump(){}

void CPlayerJump::tick(float delta){}
void CPlayerJump::getMessage(Message* m)
{

	MModVelAndJump* mModVelJump;

	switch (m->getType()){
	case MSG_PLAYER_JUMP:
		_timeCounter = SDL_GetTicks();
		if (_nJumps < _maxJumps && ((_timeCounter - _lastTimeJump) > _jumpRate)){
			//std::cout << _nJumps << std::endl;
			_nJumps++;
			pEnt->getMessage(new MRigidbodyJump(_jumpForce, pEnt->getID()));
			_lastTimeJump = SDL_GetTicks();
		}
		break;
	case MSG_COLLISION_TERRAIN:
		_nJumps = 0;
		break;

	case MSG_MOD_VEL_JUMP:
		mModVelJump = static_cast<MModVelAndJump*>(m);
		_jumpForce = _jumpForce + (_jumpForce * mModVelJump->getJumpValue() / 100.0f);
		if (_jumpForce > _maxForce){
			_jumpForce = _maxForce;
		}
		break;

	case MSG_PASSMOD_DES:
		resetForceJump();
		break;

	default:
		break;

	}
}
#pragma endregion

//Basic Attack Component
#pragma region Player Basic Attack Component

CPlayerBasicAttack::CPlayerBasicAttack(Entity* father, float fireRate, E_BULLET bT, Ogre::Vector3 entPos, float damage) :GameComponent(CMP_BASIC_ATTACK, father),
_maxFireRate(MAX_FIRE_RATE), _fireRate(fireRate), _bulletType(bT), _ogrepos(entPos), _damage(damage), _auxDamageReset(damage), _auxFireRateReset(fireRate)
{
	_lastTimeShot = 0;
	_timeCounter = 0;
	_radius = 3.0f;

}
CPlayerBasicAttack::~CPlayerBasicAttack(){

}
void CPlayerBasicAttack::tick(float delta){


}
void CPlayerBasicAttack::getMessage(Message* m){

	if (m->getType() == MSG_PLAYER_SHOT){
		MPlayerShot* mPS = static_cast<MPlayerShot*>(m);

		//Check if the player can spawn the next bullet
		_timeCounter = (SDL_GetTicks());
		if ((_timeCounter - _lastTimeShot) > _fireRate){
			float angle;
			Ogre::Vector3 iniPos;
			if (mPS->getXValue() != 0 || mPS->getYValue() != 0){
				calculateSpawnPoint(mPS->getXValue(), mPS->getYValue(), angle, iniPos);
				Ogre::Vector3 dir = iniPos;
				iniPos.x += _ogrepos.x;
				iniPos.y += _ogrepos.y;
				iniPos.z = _ogrepos.z;

				Entity* b = EntityFactory::getInstance().createBullet(_bulletType, pEnt->getScene(), iniPos, angle, _damage, pEnt->getID());
				pEnt->getMessage(new MAddEntity(pEnt->getID(), b));
				pEnt->getMessage(new MShot(dir.x, dir.y, pEnt->getID()));
				b->getMessage(new MShot(dir.x, dir.y, pEnt->getID()));

			}

			_lastTimeShot = (SDL_GetTicks());
		}
	}

	else if (m->getType() == MSG_UPDATE_TRANSFORM){
		if (m->getEmmiter() == pEnt->getID()){
			//Message cast
			MUpdateTransform* msg = static_cast<MUpdateTransform *>(m);

			//We get the size of the colliderbox.
			float w = msg->getW();
			float h = msg->getH();



			//Where our node will rotate.
			Ogre::Vector3 parentPos = msg->GetPos();

			//Where our mesh is relative to the parent. The real pos of the object is the parent pos + this variable, _ogrepos.
			_ogrepos.x = parentPos.x + w / 2;
			_ogrepos.y = parentPos.y + h / 2;
			_ogrepos.z = parentPos.z;

			//Rotate the parent node the same degree as the collider.
			float angleRad = msg->getRotation();
			float grades = (angleRad * 180) / 3.14159265359f;
		}
	}


	else if (m->getType() == MSG_MOD_DMG){
		float dmgValue = static_cast<MModDmg*>(m)->getValue();
		_damage = _damage + (_damage* dmgValue / 100.0f);

#ifdef _DEBUG
		//std::cout << _damage << "\n";

#endif // DEBUG


	}

	else if (m->getType() == MSG_MOD_FIRERATE){
		float fireRateValue = static_cast<MModFireRate*>(m)->getFireRateValue();
		_fireRate = _fireRate - (_fireRate*fireRateValue / 100.0f);
		if (_fireRate < _maxFireRate){
			_fireRate = _maxFireRate;
		}

	}
	else if (m->getType() == MSG_PASSMOD_DES){
		resetDamage();
		resetFireRate();
	}

	else if (m->getType() == MSG_ACTIVEMOD_RES){
		resetDamage();
		resetFireRate();
	}
}


void CPlayerBasicAttack::calculateSpawnPoint(float vX, float vY, float &angle, Ogre::Vector3 &iniPos){

	//Calculate point
	/*5 - 327*/
	//Normalize
	if (vX == 0){
		iniPos.x = 0;
		iniPos.z = 0;
		iniPos.y = _radius;
		angle = 0.0f;

		if (vY < 0){
			iniPos.y =  - _radius;
			angle = 180.0f;
		}


	}
	else if (vY == 0){

		iniPos.x = _radius;
		iniPos.y = 0;
		iniPos.z = 0;
		angle = -90;

		if (vX < 0){
			iniPos.x =  - _radius;
			angle = 90;
		}
	}
	else{

		float normalX = vX * SPAWN_PARSE;
		float normalY = vY * SPAWN_PARSE;

		//std::cout << vX << " " << vY << std::endl;

		float A = vY;
		float B = -vX;
		float yFinal = std::sqrt(std::pow(_radius, 2) / (std::pow((-(B) / A), 2) + 1));
		float xFinal = ((-B * yFinal) / A);


		//std::cout << xFinal << " " << yFinal << std::endl;

		if (vY > 0.0f){
			if (yFinal < 0)
				yFinal = std::abs(yFinal);
		}
		else{
			if (yFinal > 0)
				yFinal *= -1.0f;
		}

		if (vX > 0.0f){
			if (xFinal < 0)
				xFinal = std::abs(xFinal);
		}
		else{
			if (xFinal > 0)
				xFinal *= -1.0f;
		}

		//std::cout << xFinal << " " << yFinal << std::endl;


		iniPos = Ogre::Vector3(xFinal, yFinal, 0.0f);
		Ogre::Vector3 idleVector(_radius, 0.0f, 0.0f);

		float escalarProduct = (iniPos.x * idleVector.x) + (iniPos.y * idleVector.y);
		float lengthIni = std::sqrt(std::pow(iniPos.x, 2) + std::pow(iniPos.y, 2));
		float lengthIdle = _radius;

		float cos = escalarProduct / (lengthIdle * lengthIni);

		if (cos > 1)
			cos = 1;
		else if (cos < -1)
			cos = -1;

		angle = std::acosf(cos);
		angle = ((angle * 180.0f) / 3.14159265359f) - 90.0f;


		if (yFinal < 0.0f){
			if (xFinal < 0.0f){
				float comp = 90.0f - angle;
				angle += comp * 2;
			}
			else{
				float comp = 90.0f + angle;
				comp = -comp;
				angle += comp * 2;
			}
		}
	}
}

#pragma endregion

//Bullet Component
#pragma region Bullet Component

CBullet::CBullet(Entity* father, E_BULLET bT, float damage, float vel) :GameComponent(CMP_BASIC_ATTACK, father), _damage(damage)
, _velocity(vel), _toDelete(false), _auxVelocityReset(vel)
{
	s = nullptr;

}
CBullet::~CBullet(){}

void CBullet::tick(float delta){}
void CBullet::getMessage(Message* m){

	//posible error de memoria dinamica


 	MShot* mShot;
	MCollisionBegin* mCollision;

	float xDir;
	float yDir;

	float velBullets;

	switch (m->getType()){

	case MSG_SHOT:
		mShot = static_cast<MShot*>(m);

		xDir = (mShot->getXValue() / 200.0f) * _velocity;
		yDir = (mShot->getYValue() / 200.0f) * _velocity;

		pEnt->getMessage(new MRigidbodyMoveX(xDir, pEnt->getID()));
		pEnt->getMessage(new MRigidbodyMoveY(yDir, pEnt->getID()));



		break;

	case MSG_COLLISION:
		if (!_toDelete){
			mCollision = static_cast<MCollisionBegin*>(m);
			mCollision->GetWho()->getMessage(new MBulletHit(_damage, mCollision->GetContactMask(), pEnt->getID()));

			pEnt->getScene()->addEntityToDelete(pEnt);

			_toDelete = true;
		}
		break;

	case MSG_MOD_VELBULLETS:
		velBullets = static_cast<MModVelBullets*>(m)->getVelBulletsValue();
		_velocity = _velocity + (_velocity * velBullets / 100.0f);
		//a�adir un maximo de vel de las balas si queremos
		break;

	case MSG_PASSMOD_DES:
		resetVelocity();
		break;

	default:
		break;
	}

}
#pragma endregion



#pragma region Ability Component
CAbility::CAbility(ComponentType c, Entity* father, float componentLife, float componentArmor, uint16 mask, int t) :GameComponent(c, father), _componentLife(componentLife),
_componentArmor(componentArmor), _limitLife(componentLife), _type(t)

{
	dead = false;
	if (pEnt->getID() == "Player_0"){
		switch (mask)
		{
		case MASK_LEGS_0:
			_myMask = MASK_LEGS_0;
			break;
		case MASK_HEAD_0:
			_myMask = MASK_HEAD_0;
			break;
		case MASK_CHEST_0:
			_myMask = MASK_CHEST_0;
			break;
		default:
			break;
		}

	}
	else
	{

		switch (mask)
		{
		case MASK_LEGS_0:
			_myMask = MASK_LEGS_1;
			break;
		case MASK_HEAD_0:
			_myMask = MASK_HEAD_1;
			break;
		case MASK_CHEST_0:
			_myMask = MASK_CHEST_1;
			break;
		default:
			break;
		}


	}

}
CAbility::~CAbility(){}

void CAbility::getMessage(Message *m){

	/*Comprobar si hay que llamar a esta funcion en cada una de las virtuales de los hijos*/

	switch (m->getType()){
	case MSG_DAMAGE_ARMATURE:
		MDamageArmature* mDA = static_cast<MDamageArmature*>(m);
		if (mDA->getWhere() == _myMask){
			_componentLife -= mDA->getDamage();
			//Si se acaba la vida pues quitarlo o lo que sea. Aclarar el significado de armadura
			float dmg = mDA->getDamage();
			if (_componentLife > 0){
				dmg = mDA->getDamage() * (1 - _componentArmor / 100.0f);
			}
			else if (!dead){
				if (_myMask == MASK_HEAD_0 || _myMask == MASK_HEAD_1)
					pEnt->getMessage(new MActiveDead(pEnt->getID()));
				else
					pEnt->getMessage(new MPassiveDead(pEnt->getID()));
				dead = true;
			}
			pEnt->getMessage(new MDamage(dmg, pEnt->getID()));
		}
		break;
	}
}
#pragma endregion

////////////iria debajo de la de life por mantener un orden
//Armor Component
/*#pragma region Armor Component
CArmor::CArmor(Entity* father, float iniArmor) :GameComponent(CMP_ARMOR, father), _maxArmor(iniArmor), _currentArmor(iniArmor){}
CArmor::~CArmor(){}

void CArmor::tick(float delta){}
void CArmor::getMessage(Message* m){}
#pragma endregion
*/

//Passive Skill Component
#pragma region CPSkill Component

GameComponent* createPassiveAbilityEmpty(Entity* father, int id){ return new CPSkillEmpty(father); }
CPSkillEmpty::CPSkillEmpty(Entity * father) :CAbility(CMP_PASSIVE_DEFAULT, father, 0, 0, MASK_LEGS_0,1){

}
CPSkillEmpty::~CPSkillEmpty(){}

void CPSkillEmpty::tick(float delta){}
void CPSkillEmpty::getMessage(Message* m){

}

GameComponent* createActiveAbilityEmpty(Entity* father, int id){ return new CASkillEmpty(father); }
CASkillEmpty::CASkillEmpty(Entity * father) :CAbility(CMP_ACTIVE_DEFAULT, father, 0, 0, MASK_HEAD_0,1){

}
CASkillEmpty::~CASkillEmpty(){}

void CASkillEmpty::tick(float delta){}
void CASkillEmpty::getMessage(Message* m){

}


///invisibility
GameComponent* createAbilityVidar(Entity* father, int id){ return new CPSkillVidar(father); }
CPSkillVidar::CPSkillVidar(Entity * father) :CAbility(CMP_PASSIVE_VIDAR, father, 25, 25,MASK_LEGS_0,1){
	pEnt->getMessage(new MModInvisibility(pEnt->getID()));
}
CPSkillVidar::~CPSkillVidar(){}

void CPSkillVidar::tick(float delta){}
void CPSkillVidar::getMessage(Message* m){
	if (m->getType() == MSG_RESTORE_LIFE_CARDS){
		_componentLife += (_componentLife / 2);
	}
	CAbility::getMessage(m);
}


///modify dmg of a god
GameComponent* createAbilityHades(Entity* father, int id){ return new CPSkillHades(father); }
CPSkillHades::CPSkillHades(Entity * father) :CAbility(CMP_PASSIVE_HADES, father, 25, 100, MASK_LEGS_0,1){
	pEnt->getMessage(new MModDmg(pEnt->getID(), 10.0f));
}
CPSkillHades::~CPSkillHades(){}

void CPSkillHades::tick(float delta){}
void CPSkillHades::getMessage(Message* m){
	if (m->getType() == MSG_RESTORE_LIFE_CARDS){
		_componentLife += (_componentLife / 2);
		if (_componentLife > _limitLife){
			_componentLife = _limitLife;
		}
	}
	CAbility::getMessage(m);
}




///modify velocity of a god
GameComponent* createAbilityUll(Entity* father, int id){ return new CPSkillUll(father); }
CPSkillUll::CPSkillUll(Entity * father) :CAbility(CMP_PASSIVE_ULL, father, 100, 100, MASK_LEGS_0,1){
	pEnt->getMessage(new MModVel(pEnt->getID(), -20.0f));
}
CPSkillUll::~CPSkillUll(){}

void CPSkillUll::tick(float delta){}
void CPSkillUll::getMessage(Message* m){
	if (m->getType() == MSG_RESTORE_LIFE_CARDS){
		_componentLife += (_componentLife / 2);
		if (_componentLife > _limitLife){
			_componentLife = _limitLife;
		}
	}
	CAbility::getMessage(m);
}


///modify vel of bullets
GameComponent* createAbilityVali(Entity* father, int id){ return new CPSkillVali(father); }
CPSkillVali::CPSkillVali(Entity * father) :CAbility(CMP_PASSIVE_VALI, father, 50, 75, MASK_LEGS_0,1){
	pEnt->getMessage(new MModVelBullets(pEnt->getID(), 10));
}
CPSkillVali::~CPSkillVali(){}

void CPSkillVali::tick(float delta){}
void CPSkillVali::getMessage(Message* m){
	if (m->getType() == MSG_RESTORE_LIFE_CARDS){
		_componentLife += (_componentLife / 2);
		if (_componentLife > _limitLife){
			_componentLife = _limitLife;
		}
	}
	CAbility::getMessage(m);
}


///modify velocity and jump of a god
GameComponent* createAbilityHermes(Entity* father, int id){ return new CPSkillHermes(father); }
CPSkillHermes::CPSkillHermes(Entity * father) :CAbility(CMP_PASSIVE_HERMES, father, 50, 25, MASK_LEGS_0,1){
	pEnt->getMessage(new MModVelAndJump(pEnt->getID(), 20.0f, 20.0f));
}
CPSkillHermes::~CPSkillHermes(){}

void CPSkillHermes::tick(float delta){}
void CPSkillHermes::getMessage(Message* m){
	if (m->getType() == MSG_RESTORE_LIFE_CARDS){
		_componentLife += (_componentLife / 2);
		if (_componentLife > _limitLife){
			_componentLife = _limitLife;
		}
	}
	CAbility::getMessage(m);
}


///modify vel of fire rate
GameComponent* createAbilitySyn(Entity* father, int id){ return new CPSkillSyn(father); }
CPSkillSyn::CPSkillSyn(Entity * father) :CAbility(CMP_PASSIVE_SYN, father, 50, 50, MASK_LEGS_0,1){
	pEnt->getMessage(new MModVelAndJump(pEnt->getID(), 20, 20));
}
CPSkillSyn::~CPSkillSyn(){}

void CPSkillSyn::tick(float delta){}
void CPSkillSyn::getMessage(Message* m){
	if (m->getType() == MSG_RESTORE_LIFE_CARDS){
		_componentLife += (_componentLife / 2);
		if (_componentLife > _limitLife){
			_componentLife = _limitLife;
		}
	}
	CAbility::getMessage(m);
}

#pragma endregion

#pragma region Shu Headdress
//Dash
GameComponent* createAbilityShuHeaddress(Entity* father, int id){ return new CShuHeaddress(father,id); }
CShuHeaddress::CShuHeaddress(Entity * father, int id) :CAbility(CMP_SHU_HEADDRESS, father, 100, 100, MASK_HEAD_0,0), _playerId(id){
	_timeCounter = _lastTimeDash = 0;
	_coolDown = 2000.0f; //5 seconds
	_dashImpulse = 1000.0f;
	_lastTimeDash = SDL_GetTicks();
}
CShuHeaddress::~CShuHeaddress(){}

void CShuHeaddress::tick(float delta){
	float val = (SDL_GetTicks() - _lastTimeDash) * 100 / _coolDown;
	if (val > 100)val = 100;
	pEnt->getMessage(new MUpdateActiveTimer(pEnt->getID(), val));


}
void CShuHeaddress::getMessage(Message* m)
{

	if (m->getType() == MSG_INPUT_STATE){
		MInputState* inputM = static_cast<MInputState*>(m);
		if (inputM->getId() == _playerId){
			_timeCounter = SDL_GetTicks();
			ControllerInputState cState = inputM->getCInputState();

			if (cState.Left_Shoulder == BTT_RELEASED && (_timeCounter - _lastTimeDash) > _coolDown){

				Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/FightScene/ActiveUse.wav");
				b2Vec2 *impulse = calculateDash(cState.Axis_LeftX,0);
				pEnt->getMessage(new MDash(pEnt->getID(), impulse));
				_lastTimeDash = SDL_GetTicks();
			}
		}
	}
	CAbility::getMessage(m);
}

b2Vec2* CShuHeaddress::calculateDash(float xValue, float yValue){

	float normalX = xValue * SPAWN_PARSE;
	float normalY = yValue * SPAWN_PARSE;

	return new b2Vec2(_dashImpulse * normalX, _dashImpulse * normalY);

}
#pragma endregion

#pragma region Jonsu Moon
//Velocity improvement
GameComponent* createAbilityJonsuMoon(Entity* father, int id){ return new CJonsuMoon(father, id); }

CJonsuMoon::CJonsuMoon(Entity * father, int id) :CAbility(CMP_JONSU_MOON, father, 100, 100, MASK_HEAD_0,0), _playerId(id){
	_timeCounter = _initTime = 0;
	_timeActiveLimit = 5000.0f; //5 seconds
	_coolDown = 10000.0f;
	_velocityPercentage = 50.0f;
	_isActive = false;
	isAvailable = true;
}
CJonsuMoon::~CJonsuMoon(){}

void CJonsuMoon::tick(float delta){

	//Whe is active and timeActiveLimit completes, deactivate it and start cooldown
	if (_isActive){
		_timeCounter = SDL_GetTicks();
		if ((_timeCounter - _initTime) >= _timeActiveLimit){
			pEnt->getMessage(new MReset(pEnt->getID()));
			_isActive = false;
			_initTime = SDL_GetTicks();
		}
	}
	//If is not active and is not available, we count the cooldown. Then turn it to available.
	else if(!isAvailable){
		_timeCounter = SDL_GetTicks();
		if ((_timeCounter - _initTime) >= _coolDown){
			isAvailable = true;
		}
		float  val = (SDL_GetTicks() - _initTime) * 100 / _coolDown;
		if (val > 100)val = 100;
		pEnt->getMessage(new MUpdateActiveTimer(pEnt->getID(), val));

	}

}
void CJonsuMoon::getMessage(Message* m)
{
	if (m->getType() == MSG_INPUT_STATE){
		MInputState* inputM = static_cast<MInputState*>(m);
		if (inputM->getId() == _playerId && isAvailable){
			ControllerInputState cState = inputM->getCInputState();
			if (cState.Left_Shoulder == BTT_RELEASED ){
				pEnt->getMessage(new MModVel(pEnt->getID(), _velocityPercentage));
				_initTime = SDL_GetTicks();
				_isActive = true;
				isAvailable = false;

				Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/FightScene/ActiveUse.wav");
				pEnt->getMessage(new MUpdateActiveTimer(pEnt->getID(), 0));
			}
		}
	}
	CAbility::getMessage(m);
}
#pragma endregion

#pragma region Khepri Beetle
//Velocity improvement
GameComponent* createAbilityKhepriBeetle(Entity* father, int id){ return new CKhepriBeetle(father, id); }
CKhepriBeetle::CKhepriBeetle(Entity * father, int id) :CAbility(CMP_KHEPRI_BEETLE, father, 100, 100, MASK_HEAD_0,0), _playerId(id){
	_timeCounter = _initTime = 0;
	_timeActiveLimit = 3000.0f; //3 seconds
	_coolDown = 10000.0f;
	_fireRatePercentage = 40.0f;
	_isActive = false;
	isAvailable = true;
}
CKhepriBeetle::~CKhepriBeetle(){}

void CKhepriBeetle::tick(float delta){

	//Whe is active and timeActiveLimit completes, deactivate it and start cooldown
	if (_isActive){
		_timeCounter = SDL_GetTicks();
		if ((_timeCounter - _initTime) >= _timeActiveLimit){
			pEnt->getMessage(new MReset(pEnt->getID()));
			_isActive = false;
			_initTime = SDL_GetTicks();
		}
	}
	//If is not active and is not available, we count the cooldown. Then turn it to available.
	else if (!isAvailable){

		_timeCounter = SDL_GetTicks();
		if ((_timeCounter - _initTime) >= _coolDown){
			isAvailable = true;
		}
		float val = (SDL_GetTicks() - _initTime) * 100 / _coolDown;
		if (val > 100)val = 100;
		pEnt->getMessage(new MUpdateActiveTimer(pEnt->getID(), val));

	}


}
void CKhepriBeetle::getMessage(Message* m)
{
	if (m->getType() == MSG_INPUT_STATE){
		MInputState* inputM = static_cast<MInputState*>(m);
		if (inputM->getId() == _playerId && isAvailable){
			ControllerInputState cState = inputM->getCInputState();
			if (cState.Left_Shoulder == BTT_RELEASED){
				pEnt->getMessage(new MModFireRate(pEnt->getID(), _fireRatePercentage));
				_initTime = SDL_GetTicks();
				_isActive = true;
				isAvailable = false;

				Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/FightScene/ActiveUse.wav");
				pEnt->getMessage(new MUpdateActiveTimer(pEnt->getID(), 0));
			}
		}
	}
	CAbility::getMessage(m);
}
#pragma endregion


#pragma region Hera�s Rune
//Velocity improvement

GameComponent* createAbilityHeraRune(Entity* father, int id){ return new CHeraRune(father, id); }
CHeraRune::CHeraRune(Entity * father, int id) :CAbility(CMP_HERA_RUNE, father, 50, 100, MASK_HEAD_0,0), _playerId(id){
	_timeCounter = _initTime = 0;
	_coolDown = 10000.0f;
	isAvailable = true;
}
CHeraRune::~CHeraRune() {}

void CHeraRune::tick(float delta) {

	if (!isAvailable) {
		_timeCounter = SDL_GetTicks();
		if ((_timeCounter - _initTime) >= _coolDown) {
			isAvailable = true;
		}

		float val = (SDL_GetTicks() - _initTime) * 100 / _coolDown;
		if (val > 100)val = 100;
		pEnt->getMessage(new MUpdateActiveTimer(pEnt->getID(), val));

	}

}

void CHeraRune::getMessage(Message* m)
{
	if (m->getType() == MSG_INPUT_STATE) {
		MInputState* inputM = static_cast<MInputState*>(m);
		if (inputM->getId() == _playerId && isAvailable) {
			ControllerInputState cState = inputM->getCInputState();
			if (cState.Left_Shoulder == BTT_RELEASED) {
				pEnt->getMessage(new MRestoreLifeCards(pEnt->getID()));
				_initTime = SDL_GetTicks();
				isAvailable = false;

				Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/FightScene/ActiveUse.wav");

				pEnt->getMessage(new MUpdateActiveTimer(pEnt->getID(), 0));
			}
		}
	}
	if (m->getType() == MSG_RESTORE_LIFE_CARDS) {
		_componentLife += (_componentLife / 2);
		if (_componentLife > _limitLife) {
			_componentLife = _limitLife;
		}
	}
	CAbility::getMessage(m);
}
#pragma endregion


#pragma region Heris Mark

GameComponent* createAbilityHerisMark(Entity* father, int id){ return new CHerisMark(father, id); }


CHerisMark::CHerisMark(Entity * father, int id) :CAbility(CMP_HERIS_MARK, father, 50, 100, MASK_HEAD_0,0), _playerId(id){
	_timeCounter = _initTime = 0;
	_coolDown = 10000.0f;
	_timeActiveLimit = 30000.0f;
	isAvailable = true;
	_isActive = false;
	_availableShots = 10;
	_maxShots = false;
}
CHerisMark::~CHerisMark() {}

void CHerisMark::tick(float delta) {


	//When is active and timeActiveLimit completes, deactivate it and start cooldown
	if (_isActive) {
		_timeCounter = SDL_GetTicks();
		if ((_timeCounter - _initTime) >= _timeActiveLimit) {
			pEnt->getMessage(new MReset(pEnt->getID())); // Mensage modificar da�o -20%
			_availableShots = 10;
			_isActive = false;
			_initTime = SDL_GetTicks();
		}
	}
	//If is not active and is not available, we count the cooldown. Then turn it to available.
	else if (!isAvailable) {

		_timeCounter = SDL_GetTicks();
		if ((_timeCounter - _initTime) >= _coolDown) {
			isAvailable = true;
		}

		float val = (SDL_GetTicks() - _initTime) * 100 / _coolDown;
		if (val > 100)val = 100;
		pEnt->getMessage(new MUpdateActiveTimer(pEnt->getID(), val));

	}

	// Maximum number of shots has been reached so deactivate the effect and start cooldown.
	if (_maxShots) {
		_isActive = false;
		_availableShots = 10;
		_maxShots = false;
		pEnt->getMessage(new MReset(pEnt->getID()));
		_initTime = SDL_GetTicks();
	}





}
void CHerisMark::getMessage(Message* m)
{
	if (m->getType() == MSG_INPUT_STATE) {
		MInputState* inputM = static_cast<MInputState*>(m);
		if (inputM->getId() == _playerId && isAvailable) {
			ControllerInputState cState = inputM->getCInputState();

			if (cState.Left_Shoulder == BTT_PRESSED) {
				pEnt->getMessage(new MModDmg(pEnt->getID(), 20.0f)); // Mensage modificar da�o +20%
				_initTime = SDL_GetTicks();
				_isActive = true;
				isAvailable = false;

				Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/FightScene/ActiveUse.wav");

				pEnt->getMessage(new MUpdateActiveTimer(pEnt->getID(), 0));
			}
		}
	}

	// Check if a shot has been made
	else if (_isActive && m->getType() == MSG_SHOT) {
		_availableShots--;
		//std::cout << _availableShots << "\n";
		if (_availableShots == 0) {
			_maxShots = true;
		}

	}
	CAbility::getMessage(m);

}
#pragma endregion

/*-------------------------------------------------------GUI COMPONENTS---------------------------------------------------------------------------*/
#pragma region GUI COMPONENTS
#pragma region ButtonGUI
CButtonGUI::CButtonGUI(ComponentType t,Ogre::Overlay * overlay, Entity * father, size_t sceneId, Ogre::Vector2 screenpos, Ogre::Vector2 pixelSize) :GameComponent(t, father), _clicked(false) {
	pOver = overlay;

	_sceneId = sceneId;


}
CButtonGUI::~CButtonGUI()
{

}
bool CButtonGUI::canClick() {

	float act = SDL_GetTicks();

	if (act - _lastClick > _minClickTime) {
		_lastClick = act;
		return true;
	}
	else return false;
}
size_t CButtonGUI::getScnId() {
	return _sceneId;
}

void CButtonGUI::tick(float delta)
{

}
void CButtonGUI::getMessage(Message * me)
{


}

#pragma endregion

#pragma region Normal Button
CNormalButton::CNormalButton(Ogre::Overlay * overlay, Entity * father, size_t sceneId, Ogre::Vector2 screenpos, Ogre::Vector2 pixelSize, ButtonCallback callback, std::string buttonTxt) :CButtonGUI(CMP_NORMAL_BUTTON,overlay, father,sceneId,screenpos,pixelSize) {
	materials[0] = "GUI/Button/Idle";
	materials[1] = "GUI/Button/Active";
	materials[2] = "GUI/Button/Click";


	_callback = callback;
	_txt = buttonTxt;


	pContainer = static_cast<Ogre::OverlayContainer *>(Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("GUI/BaseButton", "Panel", father->getID()));



	pContainer->setPosition(screenpos.x, screenpos.y);
	overlay->add2D(pContainer);

	try {
		a = static_cast<Ogre::TextAreaOverlayElement *>(pContainer->getChild(pContainer->getName() + "/GUI/BaseButton/Text"));
		a->setCaption(_txt);

	}
	catch (Ogre::Exception e) { };

}
CNormalButton::~CNormalButton()
{
	if (Ogre::OverlayManager::getSingleton().hasOverlayElement(pContainer->getName())){
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(pContainer);
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(a);
	}
}


void CNormalButton::getMessage(Message * me)
{


	if (me->getType() == MSG_GUI_BUTTON_ACTIVE)
	{
		if (static_cast<MButtonAct*>(me)->getActiveButtonIndex() == _sceneId){
			_active = true;
			pContainer->setMaterialName(materials[1]);
			Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/Buttons/Button.wav", false);
		}
		else if (_active)
		{
			_active = false;
			pContainer->setMaterialName(materials[0]);

		}


	}
	if (_active && me->getType() == MSG_GUI_BUTTON_CLICK) {
		pContainer->setMaterialName(materials[2]);
		_clicked = true;
 		_callback();
		Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/Buttons/ButtonOk.wav", false);

	}
}

#pragma endregion

#pragma region Ability Button
CAbilityButton::CAbilityButton(Ogre::Overlay * overlay, Entity * father, size_t sceneId, Ogre::Vector2 screenpos, Ogre::Vector2 pixelSize,  int playerId, ComponentType compType) :CButtonGUI(CMP_NORMAL_BUTTON, overlay, father, sceneId, screenpos, pixelSize),
_playerId(playerId), _compType(compType){

	std::string cmpName = compToString(compType, _type);
	_clicked = false;

	materials[0] = cmpName + "-IDLE";
	materials[1] = cmpName + "-ACTIVE";
	materials[2] = cmpName + "-CLICK";


	pContainer = static_cast<Ogre::OverlayContainer *>(Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("GUI/BaseButton", "Panel", father->getID() + std::to_string(SDL_GetTicks())));
	std::string _txt = "";
	try {
		a = static_cast<Ogre::TextAreaOverlayElement *>(pContainer->getChild(pContainer->getName() + "/GUI/BaseButton/Text"));
		a->setCaption(_txt);

	}
	catch (Ogre::Exception e) {  };
	pContainer->setMetricsMode(Ogre::GuiMetricsMode::GMM_RELATIVE);
	pContainer->setHorizontalAlignment(Ogre::GuiHorizontalAlignment::GHA_LEFT);
	pContainer->setVerticalAlignment(Ogre::GuiVerticalAlignment::GVA_TOP);
	pContainer->setPosition(screenpos.x, screenpos.y);
	pContainer->setDimensions(0.156, 0.42);
	pContainer->setMaterialName(materials[0]);
	overlay->add2D(pContainer);

}
CAbilityButton::~CAbilityButton()
{
	if (Ogre::OverlayManager::getSingleton().hasOverlayElement(pContainer->getName())){
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(pContainer);
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(a);
	}

}


void CAbilityButton::getMessage(Message * me)
{

	if (!_clicked){
		switch (me->getType()){
		case MSG_GUI_BUTTON_ACTIVE:
			if (static_cast<MButtonAct*>(me)->getActiveButtonIndex() == _sceneId){
				_active = true;
				pContainer->setMaterialName(materials[1]);
				Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/Buttons/Button.wav", false);

			}
			else if (_active)
			{
				_active = false;
				pContainer->setMaterialName(materials[0]);
			}
			break;
		case MSG_GUI_BUTTON_CLICK:
			if (static_cast<MButtonClick*>(me)->getId() == _sceneId){
				pContainer->setMaterialName(materials[2]);
				_clicked = true;
				Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/Buttons/ButtonOk.wav", false);
				pEnt->getScene()->getMessage(new MAbilitySet(pEnt->getID(), _playerId, _compType, _type));
			}
			break;
		case MSG_ABILITY_SETTER:
			if (_type == static_cast<MAbilitySet*>(me)->getType()){
				pContainer->setMaterialName(materials[2]);
				_clicked = true;
			}
			break;
		default:
			break;
		}
	}
}

#pragma endregion




#pragma region God Button
CGodButton::CGodButton(Ogre::Overlay * overlay, Entity * father, size_t sceneId, Ogre::Vector2 screenpos, Ogre::Vector2 pixelSize, int playerId, E_GOD god) :CButtonGUI(CMP_NORMAL_BUTTON, overlay, father, sceneId, screenpos, pixelSize),
_playerId(playerId), _god(god){

	std::string godName = pEnt->getScene()->godToString(god);

	materials[0] = godName + "-IDLE";
	materials[1] = godName + "-ACTIVE";
	materials[2] = godName + "-CLICK";




	pContainer = static_cast<Ogre::OverlayContainer *>(Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("GUI/BaseButton", "Panel", pEnt->getID()));
	
	
	std::string _txt = "";
	try {
		a = static_cast<Ogre::TextAreaOverlayElement *>(pContainer->getChild(pContainer->getName() + "/GUI/BaseButton/Text"));
		a->setCaption(_txt);

	}
	catch (Ogre::Exception e) { };
	
	pContainer->setMetricsMode(Ogre::GuiMetricsMode::GMM_RELATIVE);
	pContainer->setHorizontalAlignment(Ogre::GuiHorizontalAlignment::GHA_LEFT);
	pContainer->setVerticalAlignment(Ogre::GuiVerticalAlignment::GVA_TOP);
	pContainer->setPosition(screenpos.x, screenpos.y);
	pContainer->setDimensions(0.1041, 0.1851);
	overlay->add2D(pContainer);
	pContainer->setMaterialName(materials[0]);


}
CGodButton::~CGodButton()
{

	if (Ogre::OverlayManager::getSingleton().hasOverlayElement(pContainer->getName())){
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(pContainer);
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(a);
	}
}


void CGodButton::getMessage(Message * me)
{

	if (!_clicked){
		switch (me->getType()){
		case MSG_GUI_BUTTON_ACTIVE:
			if (static_cast<MButtonAct*>(me)->getActiveButtonIndex() == _sceneId){
				_active = true;
				pContainer->setMaterialName(materials[1]);
				Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/Buttons/Button.wav", false);

			}
			else if (_active)
			{
				_active = false;
				pContainer->setMaterialName(materials[0]);
			}
			break;
		case MSG_GUI_BUTTON_CLICK:
			if (static_cast<MButtonClick*>(me)->getId() == _sceneId){
				pContainer->setMaterialName(materials[2]);
				_clicked = true;
				Game::getInstance()->getSoundEngine()->play2D("../Media/sounds/Buttons/ButtonOk.wav", false);
				pEnt->getScene()->getMessage(new MGodSet(pEnt->getID(), _playerId, _god));
			}
			break;
		default:
			break;
		}
	}


}

#pragma endregion




#pragma region PlayerGUI
CPlayerGUI::CPlayerGUI(Entity * father, Ogre::Overlay * ov, guiPlayer plyer, E_GOD character) : GameComponent(CMP_GUI_PLAYERGUI, father),  pOverlay(ov), p(plyer)
{


	if (plyer == P1)player = "Player1";
	else player = "Player2";
	roundsWon = 0;

	//General container of the whole Player HUD
	pHud = pOverlay->getChild(player);
	pLowerHud = static_cast<Ogre::OverlayContainer *>(pHud->getChild(player + "/ActiveContainer"));

	//Specific reference to the lifebar and active bar, which we'll be using quite often
	plifeBar = static_cast<Ogre::OverlayContainer*>(pHud->getChild(player + "/LifeBar"));
	pActiveBar = static_cast<Ogre::OverlayContainer*>(pLowerHud->getChild(player + "/ActiveContainer/ActiveBar"));
	std::string charName;
	std::string iconMatName = "GUI/Icons/" ;


	switch (character) {
	case EG_RA:
		charName = "Ra";
		if (p == P1)iconMatName += "Ra_P1";
		else iconMatName += "Ra_P2";
		break;
	case EG_AHPUCH:
		charName = "Ahpuch";
		if (p == P1)iconMatName += "AhPuch_P1";
		else iconMatName += "AhPuch_P2";
		break;
	case EG_HACHIMAN:
		charName = "Hachiman";
		if (p == P1)iconMatName += "Hachiman_P1";
		else iconMatName += "Hachiman_P2";
		break;
	case EG_ZEUS:
		charName = "Zeus";
		if (p == P1)iconMatName += "Zeus_P1";
		else iconMatName += "Zeus_P2";
		break;
	}


//	std::cout << iconMatName << std::endl;
	pHud->getChild(player + "/Name")->setCaption(charName);
	pHud->getChild(player + "/Icon")->setMaterialName(iconMatName);

	LIFE_MAX_WIDTH = plifeBar->getWidth();
	ACTIVE_MAX_WIDTH = pActiveBar->getWidth();
	LIFE_MIN_WIDTH = ACTIVE_MIN_WIDTH = 0.0f;




}
CPlayerGUI::~CPlayerGUI() {}

void CPlayerGUI::tick(float delta) {
}
void CPlayerGUI::getMessage(Message * m) {

	MActiveDead* mAD;
	MPassiveDead* mPD;

	switch (m->getType())
	{
	case MSG_LIFE_STATE:
		if ((m->getEmmiter() == "Player_0" && p == P1) || (m->getEmmiter() == "Player_1" && p == P2))
			updateLifebar(static_cast<MLifeState *>(m)->getLifeState());
		break;
	case MSG_UPDATE_ACTIVETIMER :
		if (m->getEmmiter() == "Player_0" && p == P1 || m->getEmmiter() == "Player_1" && p == P2)
			updateActive(static_cast<MUpdateActiveTimer *>(m)->getActiveTimer());
		break;
	case MSG_ROUND_FINISHED:
		if (static_cast<MRoundFinished *>(m)->getWinnerId() == "Player_0" && p == P1 || static_cast<MRoundFinished *>(m)->getWinnerId() == "Player_1" && p == P2) {
			if (roundsWon == 0) {
				pHud->getChild(player+ "/Round1")->setMaterialName("GUI/RoundFull");
				roundsWon++;
			}
			else if (roundsWon == 1)
			{
				pHud->getChild(player+ "/Round2")->setMaterialName("GUI/RoundFull");
				roundsWon++;
			}
		}
		break;
	case MSG_ACTIVE_DEAD:
		if (m->getEmmiter() == "Player_0" && p == P1 || m->getEmmiter() == "Player_1" && p == P2) {
			pLowerHud->getChild(player + "/ActiveContainer/ActiveFrame")->setMaterialName("GUI/" + player + "/ActiveDead");
			pActiveBar->setWidth(0);
		}
		break;
	case MSG_PASSIVE_DEAD:

		if (m->getEmmiter() == "Player_0" && p == P1 || m->getEmmiter() == "Player_1" && p == P2)
			pLowerHud->getChild(player + "/ActiveConainer/PassiveIcon")->setMaterialName("GUI/PassiveDown");
		break;
	case MSG_RESET_GUI:

		pActiveBar->setMaterialName("GUI/" + player + "/ActiveBar");

		pLowerHud->getChild(player + "/ActiveConainer/PassiveIcon")->setMaterialName("GUI/PassiveUp");
		updateLifebar(LIFE_MAX_WIDTH);
		updateActive(ACTIVE_MAX_WIDTH);


		break;



	}
}

void CPlayerGUI::updateActive(size_t val)
{
	float newVal = (ACTIVE_MAX_WIDTH * val) / 100;
	if (newVal < ACTIVE_MIN_WIDTH)newVal = ACTIVE_MIN_WIDTH;
	if (p == P1)
		pActiveBar->setWidth(newVal);
	else {
		Ogre::Real newX = pActiveBar->getLeft();
		newX = (newX + pActiveBar->getWidth()) - newVal;
		pActiveBar->setLeft(newX);
		pActiveBar->setWidth(newVal);

	}

}
void CPlayerGUI::updateLifebar(size_t val) {

		float newVal = (LIFE_MAX_WIDTH * val) / 100;
		if (newVal < LIFE_MIN_WIDTH)newVal = LIFE_MIN_WIDTH;
		if(p == P1)
		plifeBar->setWidth(newVal);
		else {
			Ogre::Real newX = plifeBar->getLeft();
			newX = (newX + plifeBar->getWidth()) - newVal;
			plifeBar->setLeft(newX);
			plifeBar->setWidth(newVal);
		}
}

#pragma endregion

#pragma region CGUITimer

CGUITimer::CGUITimer(Entity* father, Ogre::Overlay * overlay):GameComponent(CMP_GUI_TIMER, father)
{
	pTimer = overlay->getChild("GUI/TimerPanel");
}

CGUITimer::~CGUITimer()
{
}
void CGUITimer::tick(float delta)
{

}
void CGUITimer::getMessage(Message * m)
{
	if (m->getType() == MSG_UPDATE_SCENETIMER) {
		pTimer->getChild("TimerPanel/TextArea")->setCaption(std::to_string(static_cast<MUpdateSceneTimer *>(m)->getSceneTimer()));
	}
}
#pragma endregion





#pragma region Camera Follow
CCameraFollow::CCameraFollow(Entity * father):GameComponent(CMP_CAMERA_FOLLOW,father){}
CCameraFollow::~CCameraFollow(){}

void CCameraFollow::tick(float delta){

	pEnt->getMessage(new MCameraFollow(_nPos, pEnt->getID()));

}
void CCameraFollow::getMessage(Message* m){

	if (m->getType() == MSG_UPDATE_TRANSFORM){
		_nPos = static_cast<MUpdateTransform*>(m)->GetPos();
	}

}
#pragma endregion
