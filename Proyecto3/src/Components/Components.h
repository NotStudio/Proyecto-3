

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <Ogre.h>
#include <string>
#include <list>

class Entity;
class Message;


typedef enum componentType {
	//to be deleted
	STRING_COMPONENT,
	MESSAGESEND_COMPONENT,
	MESH_RENDER_COMPONENT

};

//Basic gameComponent class from which every other component will inherit.
class gameComponent
{
public:
	
	virtual ~gameComponent();
	//Update method
	virtual void tick(float delta) = 0;
	//Gets a message from the entity and acts according to it
	virtual void getMessage(Message *) = 0;

	//Getters and setters
	bool getActive();
	void setActive(bool nw);
	componentType getID();


  


protected:
	//Used to send a message from the component
	void sendMessage(Message *);

	//Constructor protected so that only
	//other game components inheriting from this class can use it
	gameComponent(componentType id, Entity * ent);

	//pointer to the components' entity
	Entity * pEnt;
	//basic atributes of the component
	componentType _id;
	bool _active;

};
/*-------------------------DEBUG COMPONENTS------------------------------------*/
class stringComponent : public gameComponent
{
public:
	stringComponent(Entity *);
	~stringComponent();
	virtual void getMessage(Message * m);
	virtual void tick(float delta);

private:
	std::string whatSay;

};
class messageSendComponent:public gameComponent
{
public:
	messageSendComponent(Entity * ent);
	~messageSendComponent();
	virtual void tick(float delta);
	virtual void getMessage(Message * m);

private:
	int i;

};



/*-------------------------RENDER COMPONENTS------------------------------------*/
class renderComponent : public gameComponent
{
public:
	~renderComponent();
	Ogre::SceneNode * getSceneNode();

protected:
	renderComponent(componentType t, Entity * father, Ogre::SceneManager * scnM);
	//En el nodo de Ogre estar�n todos los valores de renderizado (x,y,z en la escena de ogre, inclinaciones, etc.)
	Ogre::SceneNode * pOgreSceneNode;
	//Puntero al sceneManager que lo ha creado
	Ogre::SceneManager * pSceneMgr;
};

class meshRenderComponent: public renderComponent
{
public:
	meshRenderComponent(std::string meshName, Entity * father, Ogre::SceneManager * scnM);
	~meshRenderComponent();

	virtual void tick(float delta);
	virtual void getMessage(Message * m);

private:
	Ogre::Entity * pOgreEnt;
};

#endif