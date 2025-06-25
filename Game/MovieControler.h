#pragma once

class AActor;

class MovieControler
{
public:
	MovieControler();
	virtual ~MovieControler() = default;

	virtual void init() abstract;
	virtual void Tick() abstract;
	virtual void Destroy() abstract;

	// Init에서 설정해줘야 할것들
public:
	void SetCameraStart(Vec3 _pos, Vec3 _rot);
	void StartMovie() { m_bStartMovie = true; }
	bool IsStart() { return m_bStartMovie; }
	bool IsEnd() { return m_bEndMovie; }

protected:
	shared_ptr<class ACameraActor> m_pMovieCamera = nullptr;

	bool m_bStartMovie = false;
	bool m_bEndMovie = false;
};


class BettyMeetControler : public MovieControler
{
public:
	void init() override;
	void Tick() override;
	void Destroy() override;

public:
	// Actor Pos
	void SetPlayerCurrentPos(Vec3 _pos) { m_vPlayerCurrentPos = _pos; }
	void SetPlayerStartPos(Vec3 _pos) { m_vPlayerStartPos = _pos; }
	void SetPlayerEndPos(Vec3 _pos) { m_vPlayerEndPos = _pos; }

	Vec3 GetPlayerPos() { return m_vPlayerCurrentPos; }

	// Track Time
	void SetTimeTrack1(float _fTime) { m_fTrack1 = _fTime; }
	void SetTimeTrack2(float _fTime) { m_fTrack2 = _fTime; }
	void SetTimeTrack3(float _fTime) { m_fTrack3 = _fTime; }
	void SetTimeTrack4(float _fTime) { m_fTrack4 = _fTime; }
	void SetUIPopUpTime(float _fTime) { m_fPopUpTime = _fTime; }
	void SetUIPopDownTime(float _fTime) { m_fPopDownTime = _fTime; }

	// Track Flag
	bool StartTrack2() { return m_bStartTrack2; }
	bool StartTrack3() { return m_bStartTrack3; }
	bool StartTrack4() { return m_bStartTrack4; }
	bool IsUIPopUp() { return m_bUIPopUp; }
	bool IsUIPopDown() { return m_bUIPopDown; }

private:
	void ResetData();

private:
	// Actor Pos
	Vec3 m_vPlayerCurrentPos = Vec3();
	Vec3 m_vPlayerStartPos = Vec3();	
	Vec3 m_vPlayerEndPos = Vec3();

	// Track Time
	float m_fTrack1 = 0.f;
	float m_fTrack2 = 0.f;
	float m_fTrack3 = 0.f;
	float m_fTrack4 = 0.f;
	float m_fPopUpTime = 0.f;
	float m_fPopDownTime = 0.f;

	// Track Flag
	bool m_bStartTrack2 = false;
	bool m_bStartTrack3 = false;
	bool m_bStartTrack4 = false;
	bool m_bUIPopUp = false;
	bool m_bUIPopDown = false;
};