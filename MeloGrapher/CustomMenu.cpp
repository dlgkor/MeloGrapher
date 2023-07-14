#include"CustomUI.h"

CustomUI::Menu::Menu() {
	default_plansize = 100.0f;
	PlaneSize = 100.0f;

	CurrentState = DEFUALT;
	NextState = NONE;

	transitionTime = 0.15f;
	accumulatedTime = 0.0f;
	targetChange = false;
	animationOn = false;

	btnnum = 4;

	stateCube[0].plane[0].p[0] = { 0,0 };
	stateCube[0].plane[0].p[1] = { -default_plansize * float(sqrt(3)) / 2, default_plansize / 2 };
	stateCube[0].plane[0].p[2] = { 0,default_plansize };
	stateCube[0].plane[0].p[3] = { default_plansize * float(sqrt(3)) / 2,default_plansize / 2 };


	stateCube[0].plane[1].p[0] = { 0,0 };
	stateCube[0].plane[1].p[1] = { default_plansize * float(sqrt(3)) / 2, default_plansize / 2 };
	stateCube[0].plane[1].p[2] = { default_plansize * float(sqrt(3)) / 2, -default_plansize / 2 };
	stateCube[0].plane[1].p[3] = { 0,-default_plansize };


	stateCube[0].plane[2].p[0] = { 0,0 };
	stateCube[0].plane[2].p[1] = { 0,-default_plansize };
	stateCube[0].plane[2].p[2] = { -default_plansize * float(sqrt(3)) / 2,-default_plansize / 2 };
	stateCube[0].plane[2].p[3] = { -default_plansize * float(sqrt(3)) / 2, default_plansize / 2 };


	stateCube[1].plane[0].p[0] = { -PlaneSize, -PlaneSize };
	stateCube[1].plane[0].p[1] = { -PlaneSize, PlaneSize };
	stateCube[1].plane[0].p[2] = { PlaneSize, PlaneSize };
	stateCube[1].plane[0].p[3] = { PlaneSize, -PlaneSize };

	stateCube[1].plane[1].p[0] = { -PlaneSize, -PlaneSize };
	stateCube[1].plane[1].p[1] = { PlaneSize, -PlaneSize };
	stateCube[1].plane[1].p[2] = { PlaneSize, -PlaneSize };
	stateCube[1].plane[1].p[3] = { -PlaneSize, -PlaneSize };

	stateCube[1].plane[2].p[0] = { -PlaneSize, -PlaneSize };
	stateCube[1].plane[2].p[1] = { -PlaneSize, -PlaneSize };
	stateCube[1].plane[2].p[2] = { -PlaneSize, PlaneSize };
	stateCube[1].plane[2].p[3] = { -PlaneSize, PlaneSize };


	stateCube[2].plane[0].p[0] = { -PlaneSize, PlaneSize };
	stateCube[2].plane[0].p[1] = { -PlaneSize, PlaneSize };
	stateCube[2].plane[0].p[2] = { PlaneSize, PlaneSize };
	stateCube[2].plane[0].p[3] = { PlaneSize, PlaneSize };

	stateCube[2].plane[1].p[0] = { -PlaneSize, PlaneSize };
	stateCube[2].plane[1].p[1] = { PlaneSize, PlaneSize };
	stateCube[2].plane[1].p[2] = { PlaneSize, -PlaneSize };
	stateCube[2].plane[1].p[3] = { -PlaneSize, -PlaneSize };

	stateCube[2].plane[2].p[0] = { -PlaneSize, PlaneSize };
	stateCube[2].plane[2].p[1] = { -PlaneSize, -PlaneSize };
	stateCube[2].plane[2].p[2] = { -PlaneSize, -PlaneSize };
	stateCube[2].plane[2].p[3] = { -PlaneSize, PlaneSize };



	stateCube[3].plane[0].p[0] = { PlaneSize, PlaneSize };
	stateCube[3].plane[0].p[1] = { -PlaneSize, PlaneSize };
	stateCube[3].plane[0].p[2] = { -PlaneSize, PlaneSize };
	stateCube[3].plane[0].p[3] = { PlaneSize, PlaneSize };

	stateCube[3].plane[1].p[0] = { PlaneSize, PlaneSize };
	stateCube[3].plane[1].p[1] = { PlaneSize, PlaneSize };
	stateCube[3].plane[1].p[2] = { PlaneSize, -PlaneSize };
	stateCube[3].plane[1].p[3] = { PlaneSize, -PlaneSize };

	stateCube[3].plane[2].p[0] = { PlaneSize, PlaneSize };
	stateCube[3].plane[2].p[1] = { PlaneSize, -PlaneSize };
	stateCube[3].plane[2].p[2] = { -PlaneSize, -PlaneSize };
	stateCube[3].plane[2].p[3] = { -PlaneSize, PlaneSize };


	for (int i = 0; i < 4; i++) {
		stateCube[i].plane[0].color = RGB(200, 200, 200);
		stateCube[i].plane[1].color = RGB(225, 225, 225);
		stateCube[i].plane[2].color = RGB(240, 240, 240);
	}

	//need to set PlaneCube

	currentCube = stateCube[0];
}

void CustomUI::Menu::ResetMenu(HWND hwnd) {
	btnlist.push_back(Button(vector2d(-PlaneSize * 0.9, 0), vector2d(-PlaneSize * 0.1, PlaneSize * 0.5), L"읽어오기", RGB(255, 255, 255), RGB(100, 100, 100), hwnd, 10));
	btnlist.push_back(Button(vector2d(PlaneSize * 0.1, 0), vector2d(PlaneSize * 0.9, PlaneSize * 0.5), L"플레이/정지", RGB(255, 255, 255), RGB(100, 100, 100), hwnd, 11));
	btnlist.push_back(Button(vector2d(-PlaneSize * 0.5, -PlaneSize * 0.5), vector2d(PlaneSize * 0.5, PlaneSize * 0.5), L"종료", RGB(255, 255, 255), RGB(100, 100, 100), hwnd, 13));
}

CustomUI::PlaneState CustomUI::Menu::DetectMouse(vector2d mousepos) {
	//need to change mouse xy center to {0,0}

	for (int i = 0; i < 3; i++) {
		if (currentCube.plane[i].Detect(mousepos))
			return (PlaneState)(i + 1);
	}

	return DEFUALT;
}

void CustomUI::Menu::TransitionAnimation(float deltaT) {
	//if transition ends >> nextstate = none
	//if transition is cancled >> currentcube go back to currenstate cube

	if (targetChange) {
		targetChange = false;
		accumulatedTime = 0.0f;
		//set velocity

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				TransformVelocity.plane[i].p[j] = (stateCube[NextState].plane[i].p[j] - stateCube[CurrentState].plane[i].p[j]) * (1 / transitionTime);
			}
		}

		animationOn = true;
	}

	if (accumulatedTime + deltaT >= transitionTime && animationOn) {
		//check if it will arive after deltaT

		switch (CurrentState)
		{
		case CustomUI::DEFUALT:
			break;
		case CustomUI::PLANE1:
			btnlist.at(2).Active = false;
			break;
		case CustomUI::PLANE2:
			btnlist.at(0).Active = false;
			btnlist.at(1).Active = false;
			break;
		case CustomUI::PLANE3:
			break;
		case CustomUI::NONE:
			break;
		default:
			break;
		}

		switch (NextState)
		{
		case CustomUI::DEFUALT:
			break;
		case CustomUI::PLANE1:
			btnlist.at(2).Active = true;
			break;
		case CustomUI::PLANE2:
			btnlist.at(0).Active = true;
			btnlist.at(1).Active = true;
			break;
		case CustomUI::PLANE3:
			break;
		case CustomUI::NONE:
			break;
		default:
			break;
		}

		currentCube = stateCube[NextState];
		CurrentState = NextState;
		NextState = NONE;

		animationOn = false;
		return;
	}

	//
	if (!animationOn)
		return;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			currentCube.plane[i].p[j] = currentCube.plane[i].p[j] + TransformVelocity.plane[i].p[j] * deltaT;
		}
	}
	accumulatedTime += deltaT;
}

void CustomUI::Menu::Update(vector2d mousepos, bool clicked) {
	//triggered when mouse move,click


	if (clicked) {
		PlaneState instant = DetectMouse(mousepos);
		if (instant != NextState) {
			if (!animationOn) {
				targetChange = true;
				NextState = instant;
			}
		}


		for (int i = 0; i < btnlist.size(); i++) {
			btnlist.at(i).Click(mousepos);
		}

		return;
	}

	//버튼개수 나중에 수정필요

	for (int i = 0; i < btnlist.size(); i++) {
		btnlist.at(i).isHighlight(mousepos);
	}

}

void CustomUI::Menu::Render(HDC hdc, float deltaT) {
	//triggered when wm_paint

	if (NextState != NONE && CurrentState != NextState) {
		TransitionAnimation(deltaT);
	}

	for (int i = 0; i < 3; i++) {
		currentCube.plane[i].Render(hdc);
	}


	if (!animationOn) {
		for (int i = 0; i < btnlist.size(); i++) {
			btnlist.at(i).Render(hdc);
		}
	}

}

void CustomUI::Menu::Render(Gdiplus::Graphics* p_graphic, float deltaT) {
	if (NextState != NONE && CurrentState != NextState) {
		TransitionAnimation(deltaT);
	}

	for (int i = 0; i < 3; i++) {
		currentCube.plane[i].Render(p_graphic);
	}

	if (!animationOn) {
		for (int i = 0; i < btnlist.size(); i++) {
			btnlist.at(i).Render(p_graphic);
		}
	}
}