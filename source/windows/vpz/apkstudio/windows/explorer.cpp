#include "explorer.hpp"

using namespace VPZ::APKStudio::Components;
using namespace VPZ::APKStudio::Helpers;
using namespace VPZ::APKStudio::Resources;

namespace VPZ {
namespace APKStudio {
namespace Windows {

Explorer::Explorer(const QString &device, QWidget *parent) :
    Dialog(parent), device(device)
{
    splitter = new QSplitter(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    createTree();
    createTabs();
    createToolbar();
    layout->addWidget(splitter);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);
    splitter->setContentsMargins(0, 0, 0, 0);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    setMinimumSize(640, 480);
    setWindowIcon(::icon("folder_stand"));
    setWindowTitle(translate("title_window").arg(device));
}

void Explorer::createTabs()
{
    tabs = new QTabWidget(splitter);
    tabs->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    tabs->setTabsClosable(true);
    splitter->addWidget(tabs);
}

void Explorer::createToolbar()
{
    QToolBar *tool_bar = new QToolBar(this);
    QAction *refresh = new QAction(::icon("arrow_circle_double"), translate("label_refresh"), tool_bar);
    QActionGroup *applications = new QActionGroup(tool_bar);
    QActionGroup *music = new QActionGroup(tool_bar);
    QActionGroup *partitions = new QActionGroup(tool_bar);
    QActionGroup *photos = new QActionGroup(tool_bar);
    QActionGroup *storage = new QActionGroup(tool_bar);
    QActionGroup *videos = new QActionGroup(tool_bar);
    applications->addAction(::icon("details"), translate("label_details"))->setData(Applications::ACTION_DETAILS);
    applications->addAction(::icon("disc_arrow"), translate("label_install"))->setData(Applications::ACTION_INSTALL);
    applications->addAction(::icon("disc_minus"), translate("label_uninstall"))->setData(Applications::ACTION_UNINSTALL);
    applications->addAction(::icon("tick"), translate("label_enable"))->setData(Applications::ACTION_ENABLE);
    applications->addAction(::icon("cross"), translate("label_disable"))->setData(Applications::ACTION_DISABLE);
    applications->addAction(::icon("arrow_top_left"), translate("label_pull"))->setData(Applications::ACTION_PULL);
    applications->setVisible(false);
    music->addAction(::icon("details"), translate("label_details"));
    music->addAction(::icon("scissors"), translate("label_move"));
    music->addAction(::icon("files"), translate("label_copy"));
    music->addAction(::icon("arrow_top_left"), translate("label_pull"));
    music->addAction(::icon("rename"), translate("label_rename"));
    music->addAction(::icon("bin_empty"), translate("label_remove"));
    music->setVisible(false);
    partitions->addAction(::icon("drive_arrow"), translate("label_remount"))->setData(Partitions::ACTION_REMOUNT);
    partitions->addAction(::icon("drive_minus"), translate("label_unmount"))->setData(Partitions::ACTION_UNMOUNT);
    partitions->setVisible(false);
    photos->addAction(::icon("details"), translate("label_details"));
    photos->addAction(::icon("scissors"), translate("label_move"));
    photos->addAction(::icon("files"), translate("label_copy"));
    photos->addAction(::icon("arrow_top_left"), translate("label_pull"));
    photos->addAction(::icon("rename"), translate("label_rename"));
    photos->addAction(::icon("bin_empty"), translate("label_remove"));
    photos->setVisible(false);
    storage->addAction(::icon("folder_plus"), translate("label_create"))->setData(Storage::ACTION_CREATE);
    storage->addAction(::icon("details"), translate("label_details"))->setData(Storage::ACTION_DETAILS);
    storage->addAction(::icon("scissors"), translate("label_move"))->setData(Storage::ACTION_MOVE);
    storage->addAction(::icon("files"), translate("label_copy"))->setData(Storage::ACTION_COPY);
    storage->addAction(::icon("arrow_top_left"), translate("label_pull"))->setData(Storage::ACTION_PULL);
    storage->addAction(::icon("arrow_bottom_right"), translate("label_push"))->setData(Storage::ACTION_PUSH);
    storage->addAction(::icon("key_solid"), translate("label_chmod"))->setData(Storage::ACTION_CHMOD);
    storage->addAction(::icon("rename"), translate("label_rename"))->setData(Storage::ACTION_RENAME);
    storage->addAction(::icon("bin_empty"), translate("label_remove"))->setData(Storage::ACTION_REMOVE);
    storage->setVisible(false);
    videos->addAction(::icon("details"), translate("label_details"));
    videos->addAction(::icon("scissors"), translate("label_move"));
    videos->addAction(::icon("files"), translate("label_copy"));
    videos->addAction(::icon("arrow_top_left"), translate("label_pull"));
    videos->addAction(::icon("rename"), translate("label_rename"));
    videos->addAction(::icon("bin_empty"), translate("label_remove"));
    videos->setVisible(false);
    tool_bar->addAction(refresh);
    tool_bar->addSeparator();
    tool_bar->addActions(applications->actions());
    tool_bar->addActions(storage->actions());
    tool_bar->addActions(music->actions());
    tool_bar->addActions(partitions->actions());
    tool_bar->addActions(photos->actions());
    tool_bar->addActions(videos->actions());
    layout()->addWidget(tool_bar);
    connections.append(connect(tabs, static_cast<void(QTabWidget::*)(int)>(&QTabWidget::currentChanged), [ applications, music, partitions, photos, refresh, storage, videos, this ] (int index) {
        applications->setVisible(false);
        music->setVisible(false);
        partitions->setVisible(false);
        photos->setVisible(false);
        refresh->setEnabled(false);
        storage->setVisible(false);
        videos->setVisible(false);
        if (index < 0)
            return;
        QWidget *widget = this->tabs->currentWidget();
        if (!widget)
            return;
        disconnect(this->actions);
        disconnect(this->chmod);
        disconnect(this->refresh);
        if (widget->inherits(Applications::staticMetaObject.className())) {
            this->actions = connect(applications, SIGNAL(triggered(QAction*)), widget, "1onAction(QAction*)");
            applications->setVisible(true);
        } else if (widget->inherits(Components::Music::staticMetaObject.className())) {
            this->actions = connect(music, SIGNAL(triggered(QAction*)), widget, "1onAction(QAction*)");
            music->setVisible(true);
        } else if (widget->inherits(Partitions::staticMetaObject.className())) {
            this->actions = connect(partitions, SIGNAL(triggered(QAction*)), widget, "1onAction(QAction*)");
            partitions->setVisible(true);
        } else if (widget->inherits(Photos::staticMetaObject.className())) {
            this->actions = connect(photos, SIGNAL(triggered(QAction*)), widget, "1onAction(QAction*)");
            photos->setVisible(true);
        } else if (widget->inherits(Storage::staticMetaObject.className())) {
            this->actions = connect(storage, SIGNAL(triggered(QAction*)), widget, "1onAction(QAction*)");
            this->chmod = connect(widget, "2showCHMOD(Resources::File)", this, "1onShowCHMOD(Resources::File)");
            storage->setVisible(true);
        } else if (widget->inherits(Videos::staticMetaObject.className())) {
            this->actions = connect(videos, SIGNAL(triggered(QAction*)), widget, "1onAction(QAction*)");
            videos->setVisible(true);
        }
        this->refresh = connect(refresh, SIGNAL(triggered()), widget, "1onRefresh()");
        refresh->setEnabled(true);
    }));
    connections.append(connect(tabs, static_cast<void(QTabWidget::*)(int)>(&QTabWidget::tabCloseRequested), [ this ] (int index) {
        this->tabs->removeTab(index);
    }));
    refresh->setEnabled(false);
}

void Explorer::createTree()
{
    tree = new TreeWidget(splitter);
    tree->header()->hide();
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setSelectionBehavior(QAbstractItemView::SelectItems);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    tree->setSortingEnabled(false);
    connections.append(connect(tree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onNodeClicked(QModelIndex))));
    splitter->addWidget(tree);
}

void Explorer::onInitComplete()
{
    QTreeWidgetItem *bookmarks = new QTreeWidgetItem(tree);
    QTreeWidgetItem *system = new QTreeWidgetItem(tree);
    QTreeWidgetItem *applications = new QTreeWidgetItem(system);
    QTreeWidgetItem *information = new QTreeWidgetItem(system);
    QTreeWidgetItem *music = new QTreeWidgetItem(bookmarks);
    QTreeWidgetItem *partitions = new QTreeWidgetItem(system);
    QTreeWidgetItem *photos = new QTreeWidgetItem(bookmarks);
    QList<int> sizes;
    QTreeWidgetItem *storage = new QTreeWidgetItem(system);
    QTreeWidgetItem *videos = new QTreeWidgetItem(bookmarks);
    applications->setIcon(0, ::icon("apk"));
    applications->setText(0, translate("navigation_applications"));
    applications->setData(0, ROLE_TYPE, NAVIGATION_APPLICATIONS);
    bookmarks->addChild(music);
    bookmarks->addChild(photos);
    bookmarks->addChild(videos);
    bookmarks->setIcon(0, ::icon("bookmark"));
    bookmarks->setText(0, translate("navigation_bookmarks"));
    information->setIcon(0, ::icon("system_monitor"));
    information->setText(0, translate("navigation_information"));
    information->setData(0, ROLE_TYPE, NAVIGATION_INFORMATION);
    music->setIcon(0, ::icon("music_beam"));
    music->setText(0, translate("navigation_music"));
    music->setData(0, ROLE_TYPE, NAVIGATION_MUSIC);
    partitions->setIcon(0, ::icon("drive"));
    partitions->setText(0, translate("navigation_partitions"));
    partitions->setData(0, ROLE_TYPE, NAVIGATION_PARTITIONS);
    photos->setIcon(0, ::icon("photo_album"));
    photos->setText(0, translate("navigation_photos"));
    photos->setData(0, ROLE_TYPE, NAVIGATION_PHOTOS);
    sizes << 160;
    sizes << (width() - (sizes.at(0) + 16));
    system->addChild(applications);
    system->addChild(information);
    system->addChild(partitions);
    system->addChild(storage);
    system->setIcon(0, ::icon("board_game"));
    system->setText(0, translate("navigation_system"));
    storage->setIcon(0, ::icon("sd_memory_card"));
    storage->setText(0, translate("navigation_storage"));
    storage->setData(0, ROLE_TYPE, NAVIGATION_STORAGE);
    videos->setIcon(0, ::icon("films"));
    videos->setText(0, translate("navigation_videos"));
    videos->setData(0, ROLE_TYPE, NAVIGATION_VIDEOS);
    splitter->setSizes(sizes);
    tree->addTopLevelItem(bookmarks);
    tree->addTopLevelItem(system);
    tree->expandAll();
    onNodeClicked(tree->index(storage));
}

void Explorer::onNodeClicked(const QModelIndex &index)
{
    QTreeWidgetItem *item = tree->item(index);
    if (!item)
        return;
    int type = item->data(0, ROLE_TYPE).value<int>();
    if (type < NAVIGATION_APPLICATIONS)
        return;
    switch (type) {
    case NAVIGATION_APPLICATIONS: {
        Applications *application = new Applications(device, tabs);
        tabs->setCurrentIndex(tabs->addTab(application, item->icon(0), item->text(0)));
        QTimer::singleShot(0, application, SLOT(onInitComplete()));
        break;
    }
    case NAVIGATION_INFORMATION: {
        Information *information = new Information(device, tabs);
        tabs->setCurrentIndex(tabs->addTab(information, item->icon(0), item->text(0)));
        QTimer::singleShot(0, information, SLOT(onInitComplete()));
        break;
    }
    case NAVIGATION_MUSIC: {
        Components::Music *music = new Components::Music(device, tabs);
        tabs->setCurrentIndex(tabs->addTab(music, item->icon(0), item->text(0)));
        QTimer::singleShot(0, music, SLOT(onInitComplete()));
        break;
    }
    case NAVIGATION_PARTITIONS: {
        Partitions *partitions = new Partitions(device, tabs);
        tabs->setCurrentIndex(tabs->addTab(partitions, item->icon(0), item->text(0)));
        QTimer::singleShot(0, partitions, SLOT(onInitComplete()));
        break;
    }
    case NAVIGATION_PHOTOS: {
        Photos *photos = new Photos(device, tabs);
        tabs->setCurrentIndex(tabs->addTab(photos, item->icon(0), item->text(0)));
        QTimer::singleShot(0, photos, SLOT(onInitComplete()));
        break;
    }
    case NAVIGATION_STORAGE: {
        Storage *storage = new Storage(device, tabs);
        tabs->setCurrentIndex(tabs->addTab(storage, item->icon(0), item->text(0)));
        QTimer::singleShot(0, storage, SLOT(onInitComplete()));
        break;
    }
    case NAVIGATION_VIDEOS: {
        Videos *videos = new Videos(device, tabs);
        tabs->setCurrentIndex(tabs->addTab(videos, item->icon(0), item->text(0)));
        QTimer::singleShot(0, videos, SLOT(onInitComplete()));
        break;
    }
    default:
        break;
    }
}

void Explorer::onShowCHMOD(const File &file)
{
    CHMOD *chmod = new CHMOD(device, file, this);
    chmod->open();
}

Explorer::~Explorer()
{
    foreach (QMetaObject::Connection connection, connections)
        disconnect(connection);
    disconnect(actions);
    disconnect(chmod);
    disconnect(refresh);
}

} // namespace Windows
} // namespace APKStudio
} // namespace VPZ