#ifndef DIRECT_SHOW_H
#define DIRECT_SHOW_H
#include <QString>
#include <QThread>
class direct_show
{
protected:
    class DirectShowVideo * player;
    uchar *pixel;
    int width;
    int height;
public:
    direct_show();
    ~direct_show();
    void playlist(const QStringList &list);
    bool loadMovie(const QString &path);
    void update();

    void close();

    void play();
    void stop();

    bool isFrameNew();
    uchar *getPixels();

    float getWidth();
    float getHeight();

    bool isPaused();
    bool isLoaded();
    bool isPlaying();

    float getPosition();
    float getSpeed();
    float getDuration();
    bool getIsMovieDone();

    void setPaused(bool bPause);
    void setPosition(float pct);
    void setVolume(float volume); // 0..1
    void setLoopState(bool state);
    void setSpeed(float speed);
    void setFrame(int frame);  // frame 0 = first frame...

    int getCurrentFrame();
    int getTotalNumFrames();
    bool getLoopState();

    void firstFrame();
    void nextFrame();
    void previousFrame();
};

class mcdata;
class mediaconv :public QThread
{
    Q_OBJECT
public:
    mediaconv ();
    ~mediaconv ();

public slots:
    void on_start_enc(const QString &srcf, const QString &dstf);
    void on_start_dec(const QString &srcf, const QString &dstf);

signals :
    void sig_update_dec(qint64, qint64 );
    void sig_update_enc(qint64, qint64 );

protected:
    void run();

private:
    mcdata *dptr;
};
#endif // DIRECT_SHOW_H
