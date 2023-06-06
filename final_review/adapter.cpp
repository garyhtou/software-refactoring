class CDPlayer
{
public:
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
	virtual play() = 0;
	virtual pause() = 0;
};

class CDAdapter : public MP3Player
{
private:
	CDPlayer *cdPlayer;

public:
	void play()
	{
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
	mp3Player->play();
	mp3Player->pause();
	return 0;
}
