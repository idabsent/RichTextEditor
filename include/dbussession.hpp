#include <QDBusConnection>
#include <QDBusAbstractAdaptor>
#include <QDBusInterface>
#include <QDBusVariant>

#include "actions.hpp"

#define SERVICE_NAME "org.example.RichText"
#define INTERFACE_NAME "org.example.RichText.events"

class DBusPublisher : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", INTERFACE_NAME)
public:
	DBusPublisher(QObject* parent);

signals:
	void action(QDBusVariant const& action);
};

class DBusSubscriber : public QDBusAbstractInterface
{
	Q_OBJECT

public:
	DBusSubscriber(QString const& service, QString const& path, QDBusConnection const& connection, QObject* parent = nullptr);

signals:
	void action(QDBusVariant const& action);
};

struct Interaction : public QObject
{
	Interaction(QObject* parent = nullptr);
	virtual void sendMessage(QDBusVariant const& msg) = 0;

signals:
	void messageReceived(QDBusVariant const& msg);

private:
	Q_OBJECT
};

struct DisabledInteraction : public Interaction
{
	void sendMessage(QDBusVariant const& msg) override
	{	}
};

struct EnabledInteraction : public Interaction
{
	EnabledInteraction(QObject* parent = nullptr);

	void initInstance(QString const& session);
	void sendMessage(QDBusVariant const& msg) override;

signals: //Only for internal use
	void action(QDBusVariant const& action);

private:
	Q_OBJECT

	DBusPublisher* m_server;
	DBusSubscriber* m_client;
	QDBusConnection m_connection;
};

struct DBusSession : public QObject
{
	void sendAction(ActionUP action);
	void sendString(QString const& str);

	static void createDetached();
	static void createDisabled();
	static void createCommon();
	static void createSession(QString const& session);
	static DBusSession* instance();

signals:
	void actionReceived(ActionType action, QByteArray const& raw);
private slots:
	void parseMessage(QDBusVariant const& msg);

private:
	Q_OBJECT

    DBusSession();
	QDBusVariant packPackage(QByteArray const& data, int appId, int actionType);

	Interaction* m_interaction;
	int m_appId;

	static DBusSession* _instance;
};
