#define OXEVENT_MAP_INDEX 113

struct tag_Quiz
{
	char level;
	char Quiz[256];
	bool answer;
};

enum OXEventStatus
{
	OXEVENT_FINISH = 0,
	OXEVENT_OPEN = 1,
	OXEVENT_CLOSE = 2,
	OXEVENT_QUIZ = 3,

	OXEVENT_ERR = 0xff
};

#ifdef ENABLE_OX_RENDER_AREA
enum OXArea
{
	OXEVENT_AREA_O,
	OXEVENT_AREA_X,

	OXEVENT_AREA_MAX
};
#endif

class COXEventManager : public singleton<COXEventManager>
{
	private :
		std::map<DWORD, DWORD> m_map_char;
		std::map<DWORD, DWORD> m_map_attender;
		std::map<DWORD, DWORD> m_map_miss;

		std::vector<std::vector<tag_Quiz> > m_vec_quiz;

		LPEVENT m_timedEvent;

	protected :
		bool CheckAnswer();

		bool EnterAudience(LPCHARACTER pChar);
		bool EnterAttender(LPCHARACTER pChar);

	public :
		bool Initialize();
		void Destroy();

		OXEventStatus GetStatus();
		void SetStatus(OXEventStatus status);

		bool LoadQuizScript(const char* szFileName);

		bool Enter(LPCHARACTER pChar);

		bool CloseEvent();

		void ClearQuiz();
		bool AddQuiz(unsigned char level, const char* pszQuestion, bool answer);
		bool ShowQuizList(LPCHARACTER pChar);

		bool Quiz(unsigned char level, int timelimit);
#ifdef ENABLE_EXTENDED_ITEM_COUNT
		bool GiveItemToAttender(DWORD dwItemVnum, short count);
#else
		bool GiveItemToAttender(DWORD dwItemVnum, BYTE count);
#endif

		bool CheckAnswer(bool answer);
		void WarpToAudience();

		bool LogWinner();

#ifdef ENABLE_OX_RENDER_AREA
		void RenderArea(OXArea eArea) const;
#endif

		DWORD GetAttenderCount() { return m_map_attender.size(); }
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
