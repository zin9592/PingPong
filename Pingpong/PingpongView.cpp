
/*
2018-05-28 마지막 체크
- 현재 발견된 문제점
1. 충돌체크가 완벽하지 않음.
   - 충돌을 가끔 2번 한다.
   - 충돌시 가끔 튕겨나가지 않고 뚫고 들어간다.

- 추가할 사항
1. 에러개선

2. 아이템 추가 필요
*/


// PingpongView.cpp: CPingpongView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Pingpong.h"
#endif

#include "PingpongDoc.h"
#include "PingpongView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPingpongView

IMPLEMENT_DYNCREATE(CPingpongView, CView)

BEGIN_MESSAGE_MAP(CPingpongView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPingpongView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CPingpongView 생성/소멸

CPingpongView::CPingpongView()
	:start(false)
	, score(0)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	srand((unsigned)time(NULL));

	b = new ball(0, 0);
	stick = new bar(0, 0);
	
	int count = 0;
	const int x = 100;
	const int y = 100;
	for (int k = 0; k < CREATE_ROW; ++k) {
		for (int i = 0; i < CREATE_BLOCK; ++i) {

			int hp = 1 + rand() % 5;
			bk[count] = new block(x + (BLOCK_WIDTH*2) * i, y + k * (BLOCK_HEIGHT * 2), hp, hp - 1);
			++count;
		}
	}


}

CPingpongView::~CPingpongView()
{
}

BOOL CPingpongView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}


/* *********************************

OnDraw 함수
각 도형들의 이동을 클라이언트에 뿌려주는 부분
마지막 확인 날짜 : 18-05-28

********************************** */
void CPingpongView::OnDraw(CDC* /*pDC*/)
{
	CPingpongDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	CClientDC dc(this);
	CRect rc;
	CDC memDC;
	CBitmap Bitmap, *oldBitmap;
	
	COLORREF c[] = { {RGB(255,0,0)},{ RGB(0,0,255) }, { RGB(255,255,255) } };
	COLORREF blockColor[] = { {RGB(255,255,255)},{ RGB(200,200,200) },{ RGB(150,150,150) }, {RGB(100,100,100)},{ RGB(50,50,50) } };

	CBrush bgColor(c[2]);

	GetClientRect(&rc);

	/* 초기 플레이어 공, 막대기 위치 지정 */
	if (b->init == false) {
		b->init = true;
		stick->init = true;
		b->x = rc.right / 2;
		b->y = rc.bottom - 30;
		stick->x = rc.right / 2;
		stick->y = rc.bottom - 10;
	}

	memDC.CreateCompatibleDC(&dc);
	Bitmap.CreateCompatibleBitmap(&dc, rc.right, rc.bottom);
	oldBitmap = (CBitmap*)memDC.SelectObject(Bitmap);
	
	memDC.SelectObject(&bgColor);
	memDC.Rectangle(0, 0, rc.right, rc.bottom);


	/* 플레이어 공 */
	CBrush ballColor(c[1]);
	memDC.SelectObject(&ballColor);
	memDC.Ellipse(b->x - b->radius, b->y - b->radius, b->x + b->radius, b->y + b->radius);

	/* 플레이어 막대기 */
	CBrush barColor(c[0]);
	memDC.SelectObject(&barColor);
	memDC.Rectangle(stick->x - stick->width, stick->y - 5, stick->x + stick->width, stick->y + 5);

	/* 부숴야 할 벽돌들 */
	CBrush bs_array[CREATE_BLOCK*CREATE_ROW];
	for (int i = 0; i < CREATE_BLOCK*CREATE_ROW; ++i) {
		if (bk[i]->show) {
			bs_array[i].CreateSolidBrush(blockColor[bk[i]->color]);
			memDC.SelectObject(&bs_array[i]);
			memDC.Rectangle(bk[i]->x - bk[i]->width, bk[i]->y - bk[i]->height, bk[i]->x + bk[i]->width, bk[i]->y + bk[i]->height);
		}
	}

	/* 플레이어의 점수판 */
	CRect scoreBoard = { rc.right - 200,rc.top + 10,rc.right,rc.top + 110 };
	CFont m_font;
	CString str;
	str.Format(L"현재 점수 : %d점", score);
	m_font.CreatePointFont(180, L"고딕");
	memDC.SelectObject(&m_font);
	memDC.DrawText(str, scoreBoard, DT_SINGLELINE);

	dc.BitBlt(0, 0, rc.right, rc.bottom, &memDC, 0, 0, SRCCOPY);


}

/* ************************************************************************** */
// CPingpongView 인쇄

void CPingpongView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPingpongView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CPingpongView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CPingpongView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

// CPingpongView 진단

#ifdef _DEBUG
void CPingpongView::AssertValid() const
{
	CView::AssertValid();
}

void CPingpongView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPingpongDoc* CPingpongView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPingpongDoc)));
	return (CPingpongDoc*)m_pDocument;
}
#endif //_DEBUG

/* ************************************************************************** */



/* *********************************

키 입력 함수
공의 이동 시작을 처리한다.
마지막 확인 날짜 : 18-05-23

********************************** */
void CPingpongView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnKeyDown(nChar, nRepCnt, nFlags);

	// 공 이동 시작
	if (nChar == VK_SPACE && start == false) {
		start = true;
		SetTimer(0, 10, NULL);
	};

	Invalidate(false);
}

/* *********************************

마우스 이동 함수
플레이어가 막대기를 움직일 수 있게 한다.
마지막 확인 날짜 : 18-05-23

********************************** */
void CPingpongView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CView::OnMouseMove(nFlags, point);

	
	CRect rc;
	GetClientRect(&rc);
	if (rc.left + stick->width <= point.x &&
		point.x <= rc.right - stick->width) {
		if (start == false) b->x = point.x;
		stick->x = point.x;
	}
	Invalidate(false);
}

/* *********************************

타이머 함수
공의 이동 및 충돌 확인을 처리하는 부분
마지막 확인 날짜 : 18-05-28

********************************** */
void CPingpongView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnTimer(nIDEvent);


	// 공 이동 타이머
	if (nIDEvent == 0) {
		b->x += b->movePerX;
		b->y += b->movePerY;

		crushBoardCheck();
		crushBarCheck();
		crushBlockCheck();
	};
	Invalidate(false);
}

/* *********************************

벽 충돌시 처리하는 함수
마지막 확인 날짜 : 18-05-28
바뀐점 : 없음

********************************** */
void CPingpongView::crushBoardCheck()
{
	CRect rc;
	GetClientRect(&rc);

	// 왼쪽 벽 충돌
	if (b->x - b->radius <= rc.left) {
		b->movePerX = b->movePerX * -1;
	}
	// 오른쪽 벽 충돌
	else if (b->x + b->radius >= rc.right) {
		b->movePerX = b->movePerX * -1;
	}
	// 위쪽 벽 충돌
	else if (b->y - b->radius <= rc.top) {
		b->movePerY = b->movePerY * -1;
	}
	// 아래쪽 벽 충돌 (게임오버)
	else if (b->y + b->radius >= rc.bottom) {
		KillTimer(0);
		MessageBox(L"Gave Over");
	}
}

/* *********************************

막대기 충돌시 처리하는 함수
마지막 확인 날짜 : 18-05-28
바뀐점 : 막대에 부딪힌 위치마다 날아가는 방향이 달라짐.

********************************** */
void CPingpongView::crushBarCheck()
{
	
	if ((stick->x - stick->width <= b->x) && (b->x <= stick->x + stick->width)) {	//위쪽 면 충돌 확인
		if (stick->y - 5 < b->y + b->radius) {	// 원과 막대가 접촉한다면
			double dx = stick->x - b->x;

			if (dx > 0) {		// 공은 현재 막대기 중심 기준 좌측편에 있습니다.
				int updateX = ceil(dx / stick->width * 5) * -1;
				b->movePerX = updateX;
			}
			else if (dx < 0) {	// 공은 현재 막대기 중심 기준 우측편에 있습니다.
				int updateX = ceil(dx / stick->width * 5) * -1;
				b->movePerX = updateX;
			}
			else {				//공과 막대기의 중심이 일치합니다.
				b->movePerX = 0;
			}
			b->movePerY = b->movePerY * -1;
		}
	}
}


/* *********************************

벽돌 충돌시 처리하는 함수
마지막 확인 날짜 : 18-05-28
문제점 : 아직도 뚫고 들어가는 문제가 있음

********************************** */
#include <vector>
#include <algorithm>
void CPingpongView::crushBlockCheck()
{
	std::vector<block*> v;
	std::vector<block*> rangeX; // 비교할 범위만 들어간 벡터
							   // bx - r - w <= bkx <= bx + r + w 만 비교

	v.assign(bk, bk + CREATE_BLOCK*CREATE_ROW);


	std::vector<block*>::iterator it = v.begin();
	for (it; it != v.end(); ++it) {

		
		if (b->x - b->radius - v[0]->width <= it[0]->x &&
			it[0]->x <= b->x + b->radius + v[0]->width && it[0]->show) {
			rangeX.push_back(*it);
		}
	}

	if(rangeX.size() != 0){
		it = rangeX.begin();
		int check = 0;
		for (it; it != rangeX.end(); ++it) {
			CRect extendRect;				// 확장된 사각형
			extendRect.left = it[0]->x - it[0]->width - b->radius;
			extendRect.right = it[0]->x + it[0]->width + b->radius;
			extendRect.top = it[0]->y - it[0]->height - b->radius;
			extendRect.bottom = it[0]->y + it[0]->height + b->radius;
			check = 0;						// 충돌을 안했는가?

			if (extendRect.top <= b->y &&	// 확장된 사각형 안에 원의 y좌표가 들어왔는가?
				b->y <= extendRect.bottom) 
			{
				// check = 1 or 2;
				if (it[0]->y + it[0]->height < b->y && it[0]->x - it[0]->width < b->x && b->x < it[0]->x + it[0]->width){			// 하
					check = 1;
					b->y += 3;
				}
				else if (it[0]->y - it[0]->height > b->y && it[0]->x - it[0]->width < b->x && b->x < it[0]->x + it[0]->width) {		// 상
					check = 1;
					b->y -= 3;
				}
				else if (it[0]->x + it[0]->width < b->x && it[0]->y - it[0]->height <= b->y && b->y <= it[0]->y + it[0]->height) {	// 우
					check = 2;
					b->x += 3;
				}
				else if (it[0]->x - it[0]->width > b->x && it[0]->y - it[0]->height <= b->y && b->y <= it[0]->y + it[0]->height) {	// 좌
					check = 2;
					b->x -= 3;
				}
			}
			else if (check == 0) {
				CPoint rectPoint[4];		// 사각형의 각 꼭지점
				int dx = 0;					// 각 꼭지점과 원과의 거리차

				rectPoint[0].x = it[0]->x - it[0]->width;		//좌상단
				rectPoint[0].y = it[0]->y - it[0]->height;
				rectPoint[1].x = it[0]->x + it[0]->width;		//우상단
				rectPoint[1].y = it[0]->y - it[0]->height;
				rectPoint[2].x = it[0]->x - it[0]->width;		//좌하단
				rectPoint[2].y = it[0]->y + it[0]->height;
				rectPoint[3].x = it[0]->x + it[0]->width;		//우하단
				rectPoint[3].y = it[0]->y + it[0]->height;

				for (int i = 0; i < 4; ++i) {
					dx = sqrt(pow((rectPoint[i].x - b->x), 2) + pow((rectPoint[i].y - b->y), 2));
					if (dx <= b->radius) {	// 꼭지점이 원 안에 있다면 충돌
						check = 3;
						switch (i) {
						case 0:
							b->x -= 3;
							b->y -= 3;
							break;
						case 1:
							b->x += 3;
							b->y -= 3;
							break;
						case 2:
							b->x -= 3;
							b->y += 3;
							break;
						case 3:
							b->x += 3;
							b->y += 3;
							break;
						}
						break;
					}
				}
			}

			if (check) {		// 충돌 시 방향전환
				//double degree = atan2f(b->movePerY, b->movePerX) * 180 / PI;
				//b->movePerX = -cos(getRadian(180 - degree)) * 5;
				//b->movePerY = -sin(getRadian(180 - degree)) * 5;
				switch (check) {
				case 1:		// 윗, 아랫면
					//b->movePerX = -cos(getRadian(180 - degree)) * 5;
					//b->movePerY = -sin(getRadian(180 - degree)) * 5;
					b->movePerY = b->movePerY * -1;
					break;
				case 2:		// 옆면
					//b->movePerX = -cos(getRadian(-degree)) * 5;
					//b->movePerY = -sin(getRadian(-degree)) * 5;
					b->movePerX = b->movePerX * -1;
					break;
				case 3:		// 꼭지점
					b->movePerX = b->movePerX  * -1;
					b->movePerY = b->movePerY * -1;
					break;
				}


				// 블록의 체력 감소
				if (it[0]->hp > 0) {
					it[0]->hp -= 1;
					it[0]->color -= 1;
					score += 100;
					if (it[0]->hp == 0) {
						it[0]->show = false;
					}
				}

				break;
			}
		}
	}
}


//double CPingpongView::getRadian(double _round)
//{
//	return _round * ( PI / 180 );
//}
