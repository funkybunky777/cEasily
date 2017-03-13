#include "../All.h"
#include "Pane.h"
#include "Cursor.h"
#include "Frame.h"
#include "Dialog.h"
#include "Text.h"
#include "TextArea.h"
#include "Button.h"

//type hit
#define GUI 0
#define DLG 1

//widget type
#define BUTTON 1
#define TEXTAREA 2

//widget node
typedef struct Node{
	bool clean;
	int type;
	void* widget;
}Node;

//exceptions
extern void sj_handleException(bool fatal, bool display, const char* msg, const char* tech);//main.c

//struct functions
static int addText(char* str, bool bgColor);
static int addButton(char* str);
static void setSize(int index, int w, int h);
static void setLoc(int index, int x, int y);
static void show(void);
static void draw(void);

//dialog functions
void sj_alert(char* msg);
void sj_dlgEvent(GuiEvent* ge);

//forward declarations
void unclean(void);

//variables
static Cursor* cursor;
static Frame* frame;
static Dialog* dlg;
static List* widList;
static int checkHit = GUI;


void sj_getPane(Pane* pane){
	cursor = sj_newCursor();
	frame = sj_newFrame();
	dlg = sj_newDialog(frame->map);
	widList = sj_newList();
	
	pane->addText = addText;
	pane->addButton = addButton;
	pane->setSize = setSize;
	pane->setLoc = setLoc;
	pane->show = show;
	pane->draw = draw;
}
void sj_freePane(void){
	sj_freeFrame(frame);
	int i;
	for(i = 0; i < widList->count; i++){
		Node* tmp = (Node*)widList->get(widList, i);	
		switch(tmp->type){
			case BUTTON:
				{
					Button* b = ((Button*)tmp->widget);
					sj_freeButton(b);
				}
				break;	
			case TEXTAREA:
				{
					TextArea* ta = ((TextArea*)tmp->widget);
					sj_freeTextArea(ta);
				}
				break;	
		
			default:
				break;
		}
	}
	sj_freeList(widList);
}

//add widgets
static int addText(char* str, bool isLabel){
	Text* t = sj_newText(str);
	TextArea* ta = sj_newTextArea(t, frame->map, isLabel);
	
	Node* node = malloc(sizeof(Node));
	if(node == NULL) sj_handleException(true, false, "Failed to malloc Node for TextArea.", NULL);
		
	int id = widList->count;
	node->clean = false;
	node->type = TEXTAREA;
	node->widget = ta;	
	widList->add(widList, node);
	
	return id;
}
static int addButton(char* str){
	Node* node = malloc(sizeof(Node));
	if(node == NULL) sj_handleException(true, false, "Failed to malloc Node for button.", NULL);
		
	int id = widList->count;
	node->clean = false;
	node->type = BUTTON;
	node->widget = sj_newButton(id, frame->map, sj_newStr(str));	
	widList->add(widList, node);	
	
	return id;
}

//alter widgets
static void setSize(int index, int w, int h){
	Node* tmp = (Node*)widList->get(widList, index);		
	
	switch(tmp->type){
	case BUTTON:
		{			
			Button* b = ((Button*)tmp->widget);
			b->setSize(b, w, h);
		}
		break;
	case TEXTAREA:
		{
			TextArea* ta = ((TextArea*)tmp->widget);
			ta->setSize(ta, w, h);
		}
		break;	
		
	default:
		break;
	
	}
}

static void setLoc(int index, int x, int y){
	int xpos, ypos;
	if(x < 0) xpos = ini.width + x; else xpos = x;
	if(y < 0) ypos = ini.height + y; else ypos = y;
			
	Node* tmp = (Node*)widList->get(widList, index);
		
	switch(tmp->type){
	case BUTTON:
		{
			Button* b = ((Button*)tmp->widget);
			b->setLoc(b, xpos, ypos);
		}
		break;
	case TEXTAREA:
		{
			TextArea* ta = ((TextArea*)tmp->widget);
			ta->setLoc(ta, xpos, ypos);
		}
		break;	
	
	default:
		break;	
	}	
}


//dialogs
void unclean(void){
	int i;
	for(i = 0; i < widList->count; i++){
		Node* tmp = (Node*)widList->get(widList, i);
		tmp->clean = false;
	}
}
void sj_alert(char* msg){
	checkHit = DLG;
	unclean();
	dlg->alert(msg);	
}
void sj_dlgEvent(GuiEvent* ge){
	dlg->dlgEvent(ge);	
	show();
	checkHit = GUI;
}

//update and draw widgets
static void show(){
	int i;
	for(i = 0; i < widList->count; i++){
		Node* tmp = (Node*)widList->get(widList, i);
		if(tmp->clean) continue;
		
		switch(tmp->type){
		case BUTTON:
			{
				Button* b = ((Button*)tmp->widget);
				b->build(b, false);	
			}
			break;	
		case TEXTAREA:
		{
			TextArea* ta = ((TextArea*)tmp->widget);
			ta->build(ta);
		}
		break;		
					
		default:
			break;
		}		
	}	
}

static void draw(void){
	switch(checkHit){
	case GUI:
		{
			int i;
			for(i = 0; i < widList->count; i++){
				Node* tmp = (Node*)widList ->get(widList, i);
				switch(tmp->type){
					case BUTTON:
						{
							Button* b = ((Button*)tmp->widget);
							b->checkHit(b);
						}
						break;
					case TEXTAREA:
						{
							TextArea* ta = ((TextArea*)tmp->widget);
							ta->update(ta);
						}
						break;	
												
					default:
						break;
				}
			}
		}
		break;
	
	case(DLG):
		{
			dlg->checkHit();	
		}
		break;
		
	default:
		break;
	}
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	frame->draw(frame);
	cursor->draw();
	glDisable(GL_BLEND);
}



