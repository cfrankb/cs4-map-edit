#include "tilebox.h"
#include <stdint.h>
#include <QDebug>
#include <QGridLayout>
#include <QToolButton>
#include <QAction>
#include "shared/qtgui/qthelper.h"
#include "shared/qtgui/qfilewrap.h"
#include "shared/FrameSet.h"
#include "shared/Frame.h"
#include "tilesdata.h"

CTileBox::CTileBox(QWidget *parent) :
    QToolBox(parent)
{
    m_tile = 0;
    setupToolbox();
}

CTileBox::~CTileBox()
{
    if (m_buttons) {
        delete [] m_buttons;
    }
}

void CTileBox::setupToolbox(){
    setMinimumSize(QSize(MAX_WIDTH,MAX_HEIGHT));
    setMaximumSize(QSize(MAX_WIDTH,MAX_HEIGHT));

    QFileWrap file;
    if (file.open(":/data/cs4edit.obl", "rb")) {
        qDebug("reading tiles");
        CFrameSet fs;
        if (fs.extract(file)) {
            qDebug("extracted: %d", fs.getSize());
        }
        file.close();

        const QString labels[] = {
            tr("Background"), tr("Essentials"), tr("Walls"),
            tr("Pickup"), tr("Monsters"), tr("Keys && doors"),
            tr("Mechanic")
        };
        uint8_t icons[] = {
            TILES_NEW_PLANT, TILES_PLAYER, TILES_BRICK,
            TILES_CHEST, TILES_OCTOPUS, TILES_HEARTDOOR,
            TILES_SWITCH_UP
        };

        const int tabCount = sizeof(icons);
        const int labelCount = sizeof(labels) /  sizeof(labels[0]);
        QWidget *w[tabCount];
        if (tabCount != labelCount) {
            qWarning("Mismatch count between icons and labels.");
        }

        for (uint8_t i=0; i < tabCount; ++i) {
            //qDebug("creating tab %u", i);
            w[i] = new QWidget(this);
            auto cw = w[i];
            auto layout = new QGridLayout(cw);
            layout->setAlignment(Qt::AlignTop);
            cw->setLayout(layout);
            auto icon = frame2icon(* fs[icons[i]]);
            this->addItem(w[i], icon, labels[i]);
        }

        int btnCount = fs.getSize();
        m_buttons = new QToolButton *[btnCount];

        const int tiles =  fs.getSize();
        for (int i=0; i < tiles; ++i) {
             //qDebug("adding tile %.2x of %.2x", i, tiles);
             auto icon = frame2icon(* fs[i]);
             const tiledef_t &def = getTileDef(i);
             if (def.hidden) {
                 continue;
             }
             int j = getTabId(def.type);
             //qDebug("tabid: %d", j);
             auto gridLayout = reinterpret_cast<QGridLayout*>(w[j]->layout());
             int count = gridLayout->count();
             auto btn = new QToolButton(this);
             m_buttons[i] = btn;
             auto action = new QAction(icon, def.basename, this);
             action->setData(i);
             gridLayout->addWidget(btn, count / MAX_COLS, count % MAX_COLS);
             btn->setDefaultAction(action);
             connect(btn, SIGNAL(triggered(QAction *)), this, SLOT(buttonPressed(QAction *)));
        }
    }
    m_buttons[m_tile]->setStyleSheet(highlightStyle());
}

const QString & CTileBox::highlightStyle() {
    static QString stylesheet = QString("* { background-color: rgb(%1,%2,%3) }")
            .arg(HIGHLIGHT_RED).arg(HIGHLIGHT_GREEN).arg(HIGHLIGHT_BLUE);
    return stylesheet;
}

void CTileBox::buttonPressed(QAction *action)
{
    int oldTile = m_tile;
    m_buttons[oldTile]->setStyleSheet("");
    m_tile = action->data().toInt();
    emit tileChanged(m_tile);
    m_buttons[m_tile]->setStyleSheet(highlightStyle());
}

void CTileBox::setTile(int tile)
{
    int oldTile = m_tile;
    m_buttons[oldTile]->setStyleSheet("");
    m_tile = tile;
    m_buttons[m_tile]->setStyleSheet(highlightStyle());
    const tiledef_t & def = getTileDef(m_tile);
    setCurrentIndex(getTabId(def.type));
}

int CTileBox::getTabId(int typeId)
{
    enum {
        TAB_BACKGROUND,
        TAB_ESSENTIALS,
        TAB_WALLS,
        TAB_PICKUP,
        TAB_MONSTERS,
        TAB_LOCKS,
        TAB_MECHANIC
    };

    int j = TAB_BACKGROUND;
    switch (typeId) {
    case TYPE_BACKGROUND:
    case TYPE_STOP:
    case TYPE_SWAMP:
        j = TAB_BACKGROUND;
        break;
    case TYPE_PLAYER:
    case TYPE_DIAMOND:
        j = TAB_ESSENTIALS;
        break;
    case TYPE_DOOR:
    case TYPE_KEY:
        j = TAB_LOCKS;
        break;
    case TYPE_DRONE:
    case TYPE_MONSTER:
    case TYPE_VAMPLANT:
        j = TAB_MONSTERS;
        break;
    case TYPE_PICKUP:
        j = TAB_PICKUP;
        break;
    case TYPE_WALLS:
        j = TAB_WALLS;
        break;
    case TYPE_LADDER:
    case TYPE_PLATFORM:
    case TYPE_SWITCH:
    case TYPE_PULLEY:
    case TYPE_TRANS_DEST:
    case TYPE_TRANS_SOURCE:
    case TYPE_SOCKET:
        j = TAB_MECHANIC;
    };
    return j;
}
