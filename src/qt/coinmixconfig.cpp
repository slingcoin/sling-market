#include "coinmixconfig.h"
#include "ui_coinmixconfig.h"

#include "bitcoinunits.h"
#include "guiconstants.h"
#include "init.h"
#include "optionsmodel.h"
#include "walletmodel.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

CoinMixConfig::CoinMixConfig(QWidget* parent) : QDialog(parent),
                                                        ui(new Ui::CoinMixConfig),
                                                        model(0)
{
    ui->setupUi(this);

    connect(ui->buttonBasic, SIGNAL(clicked()), this, SLOT(clickBasic()));
    connect(ui->buttonHigh, SIGNAL(clicked()), this, SLOT(clickHigh()));
    connect(ui->buttonMax, SIGNAL(clicked()), this, SLOT(clickMax()));
}

CoinMixConfig::~CoinMixConfig()
{
    delete ui;
}

void CoinMixConfig::setModel(WalletModel* model)
{
    this->model = model;
}

void CoinMixConfig::clickBasic()
{
    configure(true, 1000, 2);

    QString strAmount(BitcoinUnits::formatWithUnit(
        model->getOptionsModel()->getDisplayUnit(), 1000 * COIN));
    QMessageBox::information(this, tr("CoinMix Configuration"),
        tr(
            "CoinMix was successfully set to basic (%1 and 2 rounds). You can change this at any time by opening Sling's configuration screen.")
            .arg(strAmount));

    close();
}

void CoinMixConfig::clickHigh()
{
    configure(true, 1000, 8);

    QString strAmount(BitcoinUnits::formatWithUnit(
        model->getOptionsModel()->getDisplayUnit(), 1000 * COIN));
    QMessageBox::information(this, tr("CoinMix Configuration"),
        tr(
            "CoinMix was successfully set to high (%1 and 8 rounds). You can change this at any time by opening Sling's configuration screen.")
            .arg(strAmount));

    close();
}

void CoinMixConfig::clickMax()
{
    configure(true, 1000, 16);

    QString strAmount(BitcoinUnits::formatWithUnit(
        model->getOptionsModel()->getDisplayUnit(), 1000 * COIN));
    QMessageBox::information(this, tr("CoinMix Configuration"),
        tr(
            "CoinMix was successfully set to maximum (%1 and 16 rounds). You can change this at any time by opening Sling's configuration screen.")
            .arg(strAmount));

    close();
}

void CoinMixConfig::configure(bool enabled, int coins, int rounds)
{
    QSettings settings;

    settings.setValue("nCoinMixRounds", rounds);
    settings.setValue("nAnonymizeSlingAmount", coins);

    nCoinMixRounds = rounds;
    nAnonymizeSlingAmount = coins;
}
