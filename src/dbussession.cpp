#include "dbussession.hpp"
#include "tools.hpp"

#include <QDebug>
#include <QDataStream>

DBusPublisher::DBusPublisher(QObject* parent)
	: QDBusAbstractAdaptor(parent)
{
	// constructor
	setAutoRelaySignals(true);
}


DBusSubscriber::DBusSubscriber(const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent)
    : QDBusAbstractInterface(service, path, INTERFACE_NAME, connection, parent)
{
}

DBusSession* DBusSession::_instance{nullptr};

DBusSession::DBusSession()
	: QObject{nullptr}
	, m_server{nullptr}
	, m_client{nullptr}
    , m_connection{QDBusConnection::sessionBus()}
{	}

void DBusSession::initInstance(QString const& session)
{
	if (!m_connection.isConnected())
	{
		throw std::runtime_error{"Can't connect to DBus daemon. Check daemon state"};
	}

	m_appId = tools::generate_random(0, 1000);
	m_server = new DBusPublisher{ this };
    m_client = new DBusSubscriber{ {}, "/sessions/" + session, m_connection, this};

	connect(m_client, &DBusSubscriber::action, this, &DBusSession::parseMessage);

    if (!m_connection.registerObject("/sessions/" + session, this))
    {
        auto error = "Can't register object... " + m_connection.lastError().message();
        throw std::runtime_error{error.toStdString()};
	}
}

void DBusSession::createInstance(QString const& session)
{
    qDebug() << FUNC_SIGN << "|" << session;
    _instance = new DBusSession{};
    _instance->initInstance(session);
}

DBusSession* DBusSession::instance()
{
	return _instance;
}

void DBusSession::sendString(QString const& ev)
{
	qDebug() << FUNC_SIGN << "| send message" << ev;
	emit action(packPackage(ev.toUtf8(), m_appId, 100));
}

void DBusSession::sendAction(ActionUP act)
{
	auto memento = act->getMemento();
	auto data = memento->toRaw();
	auto type = static_cast<quint16>(memento->getActionType());
	emit action(packPackage(data, m_appId, type));
}

QDBusVariant DBusSession::packPackage(QByteArray const& data, int appId, int actionType)
{
	QByteArray package;
	QDataStream stream{ &package, QIODevice::WriteOnly };
	stream << appId << actionType << data;

	return QDBusVariant{ QVariant{package} };
}

void DBusSession::parseMessage(QDBusVariant const& msg)
{
	auto variant = msg.variant();
	auto data = variant.toByteArray();
	int appId{ -1 };

	QByteArray raw;
	int type{ -1 };

	QDataStream stream{ &data, QIODevice::ReadOnly };
	stream >> appId;

	if (appId == m_appId)
	{
		return;
	}
	
	stream >> type;
	stream >> raw;

	qDebug() << FUNC_SIGN << "|" << appId << type << "| bytes data received:";

	emit actionReceived(static_cast<ActionType>(type), raw);
}