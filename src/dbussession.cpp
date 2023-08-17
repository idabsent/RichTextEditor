#include "dbussession.hpp"
#include "tools.hpp"

#include <QDebug>
#include <QDataStream>

Interaction::Interaction(QObject* parent)
	: QObject{parent}
{	}

EnabledInteraction::EnabledInteraction(QObject* parent)
	: Interaction{parent}
	, m_server{ nullptr }
	, m_client{ nullptr }
	, m_connection{ QDBusConnection::sessionBus() }
{	}

void EnabledInteraction::initInstance(QString const& session)
{
	if (!m_connection.isConnected())
	{
		throw std::runtime_error{"Can't connect to DBus daemon. Check daemon state"};
	}

	m_server = new DBusPublisher{ this };
	m_client = new DBusSubscriber{ {}, "/sessions/" + session, m_connection, this };

	connect(m_client, &DBusSubscriber::action, this, &EnabledInteraction::messageReceived);

	if (!m_connection.registerObject("/sessions/" + session, this))
	{
		auto error = "Can't register object... " + m_connection.lastError().message();
		throw std::runtime_error{error.toStdString()};
	}
}

void EnabledInteraction::sendMessage(QDBusVariant const& msg)
{
	emit action(msg);
}

DBusPublisher::DBusPublisher(QObject* parent)
	: QDBusAbstractAdaptor(parent)
{
	setAutoRelaySignals(true);
}


DBusSubscriber::DBusSubscriber(const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent)
    : QDBusAbstractInterface(service, path, INTERFACE_NAME, connection, parent)
{	}

DBusSession* DBusSession::_instance{nullptr};

DBusSession::DBusSession()
	: QObject{nullptr}
	, m_interaction{nullptr}
	, m_appId{tools::generate_random(0, 1000)}
{	}

void DBusSession::createSession(QString const& session)
{
	if (_instance)
	{
		throw std::logic_error{
			QString{"%1: Attemption create exist instance"}
				.arg(FUNC_SIGN)
				.toStdString()
		};
	}

    _instance = new DBusSession{};
	auto interaction = new EnabledInteraction{ _instance };
    interaction->initInstance(session);
	_instance->m_interaction = interaction;
	connect(_instance->m_interaction, &EnabledInteraction::messageReceived, _instance, &DBusSession::parseMessage);
}

void DBusSession::createCommon()
{
	createSession("common");
}

void DBusSession::createDisabled()
{
	_instance = new DBusSession{};
	_instance->m_interaction = new DisabledInteraction{};
}

void DBusSession::createDetached()
{
	createSession("session_" + tools::generate_random(0, 1000));
}

DBusSession* DBusSession::instance()
{
	return _instance;
}

void DBusSession::sendString(QString const& ev)
{
	m_interaction->sendMessage(packPackage(ev.toUtf8(), m_appId, 100));
}

void DBusSession::sendAction(ActionUP act)
{
	auto memento = act->getMemento();
	auto data = memento->toRaw();
	auto type = static_cast<quint16>(memento->getActionType());
	m_interaction->sendMessage(packPackage(data, m_appId, type));
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

	emit actionReceived(static_cast<ActionType>(type), raw);
}
