DROP TABLE IF EXISTS `guildhouse`;

CREATE TABLE `guildhouse`
(
    `guildId` INT UNSIGNED NOT NULL,
    `ownerGuid` INT UNSIGNED NOT NULL,
    `purchaseDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (`guildId`),

    KEY `idx_owner` (`ownerGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


DROP TABLE IF EXISTS `guildhouse_asset`;

CREATE TABLE `guildhouse_asset`
(
    `assetId` INT UNSIGNED NOT NULL AUTO_INCREMENT,

    `guildId` INT UNSIGNED NOT NULL,

    `layoutId` SMALLINT UNSIGNED NOT NULL DEFAULT 1,

    `catalogId` INT UNSIGNED NOT NULL,

    `status` TINYINT UNSIGNED NOT NULL DEFAULT 0,

    `phase` INT UNSIGNED NOT NULL,

    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,

    `createdBy` INT UNSIGNED NOT NULL,

    `createdDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (`assetId`),

    KEY `idx_guild` (`guildId`),
    KEY `idx_layout` (`layoutId`),
    KEY `idx_catalog` (`catalogId`),
    KEY `idx_status` (`status`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
