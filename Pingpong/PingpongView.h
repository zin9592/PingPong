
// PingpongView.h: CPingpongView 클래스의 인터페이스
//
#define CREATE_BLOCK 20
#define CREATE_ROW 3
#define BLOCK_WIDTH 25
#define BLOCK_HEIGHT 12

#define PI 3.141592
#pragma once

class ball {
public :
	double x;
	double y;
	double movePerX;
	double movePerY;
	const int radius = 15;
	bool init;

public:
	ball(double _x, double _y) {
		x = _x;
		y = _y;
		movePerX = -5;
		movePerY = -5;
		init = false;
	}
};

class bar {
public:
	double x;
	double y;
	const int width = 50;
	bool init;

public:
	bar(double _x, double _y) {
		x = _x;
		y = _y;
		init = false;
	}
};

class block {
public:
	int x;
	int y;
	int hp;
	int color;
	const int width = BLOCK_WIDTH;
	const int height = BLOCK_HEIGHT;
	bool show;

public:
	block(int _x, int _y, int _hp, int _color) {
		x = _x;
		y = _y;
		hp = _hp;
		color = _color;
		show = true;
	}
};

class CPingpongView : public CView
{
protected: // serialization에서만 만들어집니다.
	CPingpongView();
	DECLARE_DYNCREATE(CPingpongView)

// 특성입니다.
public:
	CPingpongDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CPingpongView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
public:
	ball *b;							// 공
	bar *stick;							// 막대기
	block *bk[CREATE_BLOCK*CREATE_ROW];	// 생성될 벽돌들
	bool start;
	int score;							// 점수판

	// 충돌확인 함수
	void crushBoardCheck();		
	void crushBarCheck();
	void crushBlockCheck();

	//double getRadian(double _round);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};




#ifndef _DEBUG  // PingpongView.cpp의 디버그 버전
inline CPingpongDoc* CPingpongView::GetDocument() const
   { return reinterpret_cast<CPingpongDoc*>(m_pDocument); }
#endif

