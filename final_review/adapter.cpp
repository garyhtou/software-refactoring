class CDPlayer
{
public:
	void insertCD()
	{
		// do something
	}
	void playButton()
	{
		// do something
	}
	void pauseButton()
	{
		// do something
	}
};

// header file
class MP3Player
{
public:
	virtual play(Song song) = 0;
	virtual pause() = 0;
};

class CDAdapter : public MP3Player
{
private:
	CDPlayer *cdPlayer;

public:
	void play(Song song)
	{
		string name = song.getName();
		Cd cd = findCd(name);
		cdPlayer->insertCD(cd);
		cdPlayer->playButton();
	}
	void pause()
	{
		cdPlayer->pauseButton();
	}
};

int main()
{
	MP3Player *mp3Player = new CDAdapter();
	Song cupid = new Song("Cupid by fifty fifty");
	mp3Player->play(cupid);
	mp3Player->pause();
	return 0;
}
