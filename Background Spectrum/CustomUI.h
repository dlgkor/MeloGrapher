#pragma once

#include<windows.h>
#include<math.h>

namespace CustomUI {

	class matrix2x2 {
	public:
		float m[2][2];
	};

	class vector2d {
	public:
		float x, y;
	public:
		vector2d() :x(0.0f), y(0.0f) {}
		vector2d(float _x, float _y) :x(_x), y(_y) {}

		float Size() {
			float size = sqrt((x * x) + (y * y));
			return size;
		}

		vector2d UnitVector() {
			//단위벡터를 반환한다
			return vector2d(x / Size(), y / Size());
		}

		vector2d operator+(const vector2d& a) {
			vector2d vec(x + a.x, y + a.y);
			return vec;
		}
		vector2d operator-(const vector2d& a) {
			vector2d vec(x - a.x, y - a.y);
			return vec;
		}
		vector2d operator*(const float& a) {
			vector2d vec(x * a, y * a);
			return vec;
		}

		vector2d operator*(const matrix2x2& matrix) const {
			vector2d vec(x * matrix.m[0][0] + y * matrix.m[1][0], x * matrix.m[0][1] + y * matrix.m[1][1]);
			//matrix m[0][] i hat
			//matrix m[1][] j hat
			return vec;
		}

		float dot(const vector2d& a) {
			return x * a.x + y * a.y;
		}
	};

	class Button {
	public:
		bool Active;
		bool Highlight;
		vector2d p1, p2;
		TCHAR Btext[128];

		COLORREF defaultColor;
		COLORREF highlightColor;
		HWND parent;

		int id;

	public:
		Button() {	}

		Button(vector2d _p1, vector2d _p2, const TCHAR* _Btext,
			COLORREF _default, COLORREF _highlight, HWND _parent, int _id) {
			p1 = _p1;
			p2 = _p2;
			lstrcpy(Btext, _Btext);
			Active = false;
			Highlight = false;

			defaultColor = _default;
			highlightColor = _highlight;
			parent = _parent;

			id = _id; //number to discriminate button
		}

		void Set(vector2d _p1, vector2d _p2, const TCHAR* _Btext,
			COLORREF _default, COLORREF _highlight, HWND _parent, int _id) {
			p1 = _p1;
			p2 = _p2;
			lstrcpy(Btext, _Btext);
			Active = false;
			Highlight = false;

			defaultColor = _default;
			highlightColor = _highlight;
			parent = _parent;

			id = _id; //number to discriminate button
		}

		void SetActive(bool active) {
			Active = active;
		}

		bool DetectMouse(vector2d mousepos) {
			if (mousepos.x<p1.x || mousepos.x>p2.x)
				return false;
			if (mousepos.y<p1.y || mousepos.y>p2.y)
				return false;
			return true;
		}

		void isHighlight(vector2d mousepos) {
			if (!Active)
				return;

			if (DetectMouse(mousepos))
				Highlight = true;
			else
				Highlight = false;

			return;
		}

		void Click(vector2d mousepos) {
			if (!Active)
				return;

			if (!DetectMouse(mousepos))
				return;

			//Send Custom WindowMessage

			SendMessage(parent, WM_COMMAND, MAKEWORD(id, 0), MAKEWORD(0, 0));
		}


		void Render(HDC hdc) {
			if (!Active)
				return;

			HBRUSH HBrush, OldBrush;
			//Fill Rectangle With Color
			//Color is selected by presence of highlight
			if (Highlight) {
				HBrush = CreateSolidBrush(highlightColor);
			}
			else {
				HBrush = CreateSolidBrush(defaultColor);
			}

			OldBrush = (HBRUSH)SelectObject(hdc, HBrush);

			Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);

			SetTextAlign(hdc, TA_CENTER);
			SetBkMode(hdc, TRANSPARENT);

			//Todo : Set Text standard to middle of text
			TextOut(hdc, (p1.x + p2.x) / 2, (p1.y + p2.y) / 2, Btext, lstrlen(Btext));

			DeleteObject(SelectObject(hdc, OldBrush));
		}
	};


	class Plane {
	public:
		vector2d p[4];
		vector2d p_normal[4];
		COLORREF color;
	public:
		void UpdateNormal() {
			matrix2x2 rotation;
			rotation.m[0][0] = 0;
			rotation.m[0][1] = 1;
			rotation.m[1][0] = -1;
			rotation.m[1][1] = 0;

			for (int i = 0; i < 4; i++) {
				p_normal[i] = ((p[(i + 1) % 4] - p[i]) * rotation).UnitVector();
			}

		}
		bool IsInsideLine(vector2d normalv, vector2d point) {
			if (normalv.dot(point) < 0)
				return true;

			return false;
		}
		bool Detect(vector2d mousepos) {
			UpdateNormal();

			for (int i = 0; i < 4; i++) {
				vector2d mousev = mousepos - p[i];
				if (!IsInsideLine(p_normal[i], mousev)) {
					return false;
				}
			}
			return true;
		}
		void Render(HDC hdc) {
			HBRUSH HBrush = CreateSolidBrush(color);
			HBRUSH OldBrush;

			HPEN HPen = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
			HPEN OldPen;

			OldBrush = (HBRUSH)SelectObject(hdc, HBrush);
			OldPen = (HPEN)SelectObject(hdc, HPen);

			POINT points[4];
			for (int i = 0; i < 4; i++) {
				points[i].x = p[i].x;
				points[i].y = p[i].y;
			}
			Polygon(hdc, points, 4);

			DeleteObject(SelectObject(hdc, OldBrush));
			DeleteObject(SelectObject(hdc, OldPen));
		}
	};

	class Cube {
	public:
		Plane plane[3];
	};

	enum PlaneState {
		DEFUALT = 0, PLANE1 = 1, PLANE2 = 2, PLANE3 = 3,  NONE
	};

	class Menu {
	private:
		float PlaneSize;
		//Cube defaultCube;
		Cube stateCube[4];
		Cube currentCube;
		Cube TransformVelocity;

		PlaneState CurrentState;
		PlaneState NextState;

		int btnnum;
		//Button btnList[10];
		vector<Button> btnlist;

		float transitionTime;
		float accumulatedTime;

		bool targetChange;
		bool animationOn;
	public:
		Menu() {
			PlaneSize = 120.0f;

			CurrentState = DEFUALT;
			NextState = NONE;

			transitionTime = 0.25f;
			accumulatedTime = 0.0f;
			targetChange = false;
			animationOn = false;

			btnnum = 4;

			stateCube[0].plane[0].p[0] = { 0,0 };
			stateCube[0].plane[0].p[1] = { -PlaneSize * float(sqrt(3)) / 2, PlaneSize / 2 };
			stateCube[0].plane[0].p[2] = { 0,PlaneSize };
			stateCube[0].plane[0].p[3] = { PlaneSize * float(sqrt(3)) / 2,PlaneSize / 2 };


			stateCube[0].plane[1].p[0] = { 0,0 };
			stateCube[0].plane[1].p[1] = { PlaneSize * float(sqrt(3)) / 2, PlaneSize / 2 };
			stateCube[0].plane[1].p[2] = { PlaneSize * float(sqrt(3)) / 2, -PlaneSize / 2 };
			stateCube[0].plane[1].p[3] = { 0,-PlaneSize };


			stateCube[0].plane[2].p[0] = { 0,0 };
			stateCube[0].plane[2].p[1] = { 0,-PlaneSize };
			stateCube[0].plane[2].p[2] = { -PlaneSize * float(sqrt(3)) / 2,-PlaneSize / 2 };
			stateCube[0].plane[2].p[3] = { -PlaneSize * float(sqrt(3)) / 2, PlaneSize / 2 };


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

		void ResetMenu(HWND hwnd) {
			btnlist.push_back(Button(vector2d(-PlaneSize * 0.9, 0), vector2d(-PlaneSize * 0.1, PlaneSize*0.5), L"읽어오기", RGB(255, 255, 255), RGB(100, 100, 100), hwnd, 10));
			btnlist.push_back(Button(vector2d(PlaneSize*0.1, 0), vector2d(PlaneSize*0.9, PlaneSize*0.5), L"플레이/정지", RGB(255, 255, 255), RGB(100, 100, 100), hwnd, 11));
			btnlist.push_back(Button(vector2d(-PlaneSize*0.5, -PlaneSize*0.5), vector2d(PlaneSize*0.5, PlaneSize*0.5), L"종료", RGB(255, 255, 255), RGB(100, 100, 100), hwnd, 13));
			
		}


		PlaneState DetectMouse(vector2d mousepos) {
			//need to change mouse xy center to {0,0}

			for (int i = 0; i < 3; i++) {
				if (currentCube.plane[i].Detect(mousepos))
					return (PlaneState)(i+1);
			}

			return DEFUALT;
		}

		void TransitionAnimation(float deltaT) {
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

		void Update(vector2d mousepos, bool clicked) {
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

		void Render(HDC hdc, float deltaT) {
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
	};
}
