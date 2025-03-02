#include "dlgtest.h"
#include "ui_dlgtest.h"
#include <QShortcut>
#include <QMessageBox>
#include <QPainter>
#include <QKeyEvent>
#include "mapfile.h"
#include "game.h"
#include "tilesdata.h"
#include "Frame.h"
#include "FrameSet.h"
#include "shared/qtgui/qfilewrap.h"

CDlgTest::CDlgTest(QWidget *parent) :
    QDialog(parent),
    CGameMixin(),
    ui(new Ui::CDlgTest)
{
    ui->setupUi(this);
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(changeZoom()));
    m_timer.setInterval(1000 / TICK_RATE);
    m_timer.start();
}

CDlgTest::~CDlgTest()
{
    delete ui;
}

void CDlgTest::init(CMapFile *mapfile)
{
    m_maparch= mapfile;
    setZoom(true);
    CGameMixin::init(mapfile, mapfile->currentIndex());
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(mainLoop()));
}

void CDlgTest::exitGame()
{
    reject();
}

void CDlgTest::mainLoop()
{
    CGameMixin::mainLoop();
    update();
}

void CDlgTest::changeZoom()
{
    CGameMixin::changeZoom();
}

void CDlgTest::sanityTest()
{
    const auto level = m_game->level();
    qDebug("starting sanity test for level: %d", level);
    CMap *map = m_maparch->at(level);
    const Pos pos = map->findFirst(TILES_PLAYER);
    QStringList listIssues;
    if ((pos.x == CMap::NOT_FOUND ) && (pos.y == CMap::NOT_FOUND )) {
        listIssues.push_back(tr("No player on map"));
    } else {
        qDebug("player found");
    }
    if (map->count(TILES_DIAMOND) == 0) {
        listIssues.push_back(tr("No diamond on map"));
    } else {
        qDebug("diamonds found");
    }
    if (listIssues.count() > 0) {
        QString msg = tr("Map %1 is incomplete:\n%2").arg(m_game->level() + 1).arg(listIssues.join("\n"));
        QMessageBox::warning(this, "", msg, QMessageBox::Button::Ok);
        exitGame();
    }
    qDebug("completed sanity test");
}

void CDlgTest::setZoom(bool zoom)
{
    CGameMixin::setZoom(zoom);
    int factor = m_zoom ? 2 : 1;
    this->setMaximumSize(QSize(WIDTH * factor, HEIGHT * factor));
    this->setMinimumSize(QSize(WIDTH * factor, HEIGHT * factor));
    this->resize(QSize(WIDTH * factor, HEIGHT * factor));
}

void CDlgTest::paintEvent(QPaintEvent *)
{
    CFrame bitmap(WIDTH, HEIGHT);
    switch (m_game->mode())
    {
    case CGame::MODE_INTRO:
    case CGame::MODE_RESTART:
    case CGame::MODE_GAMEOVER:
        drawLevelIntro(bitmap);
        break;
    case CGame::MODE_LEVEL:
        drawScreen(bitmap);
    }

    // show the screen
    const QImage & img = QImage(reinterpret_cast<uint8_t*>(bitmap.getRGB()), bitmap.len(), bitmap.hei(), QImage::Format_RGBX8888);
    const QPixmap & pixmap = QPixmap::fromImage(m_zoom ? img.scaled(QSize(WIDTH * 2, HEIGHT * 2)): img);
    QPainter p(this);
    p.drawPixmap(0, 0, pixmap);
    p.end();
}

void CDlgTest::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Up:
        m_joyState[AIM_UP] = KEY_PRESSED;
        break;
    case Qt::Key_Down:
        m_joyState[AIM_DOWN] = KEY_PRESSED;
        break;
    case Qt::Key_Left:
        m_joyState[AIM_LEFT] = KEY_PRESSED;
        break;
    case Qt::Key_Right:
        m_joyState[AIM_RIGHT] = KEY_PRESSED;
        break;
    case Qt::Key_Z:
        m_joyState[Z_KEY]  = KEY_PRESSED;
        break;
    case Qt::Key_Space:
        m_joyState[BUTTON]  = KEY_PRESSED;
        break;
    case Qt::Key_Escape:
        exitGame();
    }
}

void CDlgTest::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Up:
        m_joyState[AIM_UP] = KEY_RELEASED;
        break;
    case Qt::Key_Down:
        m_joyState[AIM_DOWN] = KEY_RELEASED;
        break;
    case Qt::Key_Left:
        m_joyState[AIM_LEFT] = KEY_RELEASED;
        break;
    case Qt::Key_Right:
        m_joyState[AIM_RIGHT] = KEY_RELEASED;
        break;
    case Qt::Key_Z:
        m_joyState[Z_KEY]  = KEY_RELEASED;
        break;
    case Qt::Key_Space:
        m_joyState[BUTTON]  = KEY_RELEASED;
        break;
    }
}

void CDlgTest::preloadAssets()
{
    QFileWrap file;


    typedef struct {
        const char *filename;
        CFrameSet **frameset;
    } asset_t;

    asset_t assets[] = {
        {":/data/cs4tiles.obl", &m_tiles},
        {":/data/cs4animz.obl", &m_animz},
        {":/data/annie.obl", &m_annie},
    };

    for (int i=0; i < 3; ++i) {
        asset_t & asset = assets[i];
        *(asset.frameset) = new CFrameSet();
        if (file.open(asset.filename, "rb")) {
            qDebug("reading %s", asset.filename);
            if ((*(asset.frameset))->extract(file)) {
                qDebug("extracted: %d", (*(asset.frameset))->getSize());
            }
            file.close();
        }
    }

    const char fontName [] = ":/data/bitfont.bin";
    int size = 0;
    if (file.open(fontName, "rb")) {
        size = file.getSize();
        m_fontData = new uint8_t[size];
        file.read(m_fontData, size);
        file.close();
        qDebug("loaded font: %d bytes", size);
    } else {
        qDebug("failed to open %s", fontName);
    }
}
