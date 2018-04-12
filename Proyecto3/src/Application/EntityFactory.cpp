#include "EntityFactory.h"


EntityFactory* EntityFactory::_instance = nullptr;

//The constructor should set up the maps for all the functions
EntityFactory::EntityFactory()
{
	_idCount = 0;

	//Define all create Entity methods
	_createGodFuncs.emplace(EG_RA, &createGodRa);
}


EntityFactory::~EntityFactory()
{

}

EntityFactory& EntityFactory::getInstance(){
	if (EntityFactory::_instance == nullptr) EntityFactory::_instance = new EntityFactory();
	return *EntityFactory::_instance;
}


//Depend on the type of the entity, we call one or other function passing it the class 
Entity* EntityFactory::createEntity(E_TYPE typE, int classE, gameScene* s ){

	
	Entity* e;											//The pointer to the entity who will be returned
	std::string idS = to_string(_idCount++);			//The unique id	

	switch (typE)
	{
	case ET_GOD:
		e = createGod((E_GOD)classE, idS, s);
		break;
	default:
		break;
	}

	return e;
}

//The create god methods search in the map the func that returns the entity that E_GOD variable indicates
Entity* EntityFactory::createGod(E_GOD cE, std::string id, gameScene* s){
	
	createGodFunc c = _createGodFuncs.at(cE);
	Entity* debug = c(id, s);
	return debug;
}