#include "dlgstat.h"
#include "ui_dlgstat.h"
#include "tilesdata.h"
//#include "sprtypes.h"
#include <qlistwidget.h>
#include <qstringlist.h>

CDlgStat::CDlgStat(int tileID, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgStat)
{
    const QString types[] = {
        tr("TYPE_BACKGROUND"),
        tr("TYPE_STOP"),
        tr("TYPE_PLAYER"),
        tr("TYPE_WALLS"),
        tr("TYPE_PICKUP"),
        tr("TYPE_DOOR"),
        tr("TYPE_KEY"),
        tr("TYPE_SWAMP"),
        tr("TYPE_MONSTER"),
        tr("TYPE_DRONE"),
        tr("TYPE_VAMPLANT"),
        tr("TYPE_DIAMOND"),

        tr("TYPE_LADDER"),//                      0x0c
        tr("TYPE_PLATFORM"),//                    0x0d
        tr("TYPE_SPECIAL"),//                     0x0e
        tr("TYPE_SWITCH"),//                      0x0f
        tr("TYPE_TRANS_SOURCE"), //               0x10
        tr("TYPE_TRANS_DEST"),//                  0x11
        tr("TYPE_TRAP"),//                        0x12
        tr("TYPE_BRIDGE"),//                      0x13
        tr("TYPE_ROOT"),//                        0x14
        tr("TYPE_PULLEY"),//                      0x15
        tr("TYPE_AUTO_ROPE"),//                   0x16
        tr("TYPE_SOCKET"),//                      0x17
    };

    constexpr const size_t typeCount = sizeof(types)/ sizeof(types[0]);
    ui->setupUi(this);

    if (tileID >= TILES_MAX) {
        ui->sName->setText(tr("Invalid TileID"));
        return;
    }

    const tiledef_t & def = getTileDef(tileID);
    ui->sName->setText(def.basename);
    QListWidget * w = ui->listWidget;
    if (def.hidden) {
        w->addItem(tr("hidden"));
    }

    if (def.ai) {
        QString s = tr("AI:");
        if (def.ai & AI_ROUND) {
            s += QString(" ") + tr("ROUND");
        }

        if (def.ai & AI_STICKY) {
            s += QString(" ") + tr("STICKY");
        }

        if (def.ai & AI_FOCUS) {
            s += QString(" ") + tr("FOCUS");
        }
        w->addItem(s);
    }

    if (def.flags) {
        QString s = tr("Flags:");
        if (def.flags & FLAG_EXTRA_LIFE)
        {
            s += QString(" ") + tr("EXTRA_LIFE");
        }

        if (def.flags & FLAG_GODMODE)
        {
            s += QString(" ") + tr("GODMODE");
        }

        if (def.flags & FLAG_EXTRA_SPEED)
        {
            s += QString(" ") + tr("EXTRA_SPEED");
        }
        w->addItem(s);
    }

    if (def.health) {
        QString s = tr("Health: %1 hp").arg(def.health);
        w->addItem(s);
    }

    if (def.score) {
        w->addItem(tr("Points: +%1").arg(def.score));
    }

    if (def.speed) {
        QString s;
        switch (def.speed) {
        case SPEED_FAST:
            s = "FAST" ;
            break;
        case SPEED_NORMAL:
            s = "NORMAL" ;
            break;
        case SPEED_SLOW:
            s = "SLOW" ;
            break;
        case SPEED_VERYSLOW:
            s = "VERYSLOW" ;
            break;
        default:
            s = QString("%1").arg(def.speed);
        }

        w->addItem(tr("Speed %1").arg(s));
    }

    ui->label->setText(def.type < typeCount ? types[def.type] : "Unknown Type");
}

CDlgStat::~CDlgStat()
{
    delete ui;
}
