#include "algued/riot_client_fake.h"

namespace algue::algued {

std::optional<std::string_view> get_riot_client_fake(std::string_view url)
{
    if (url == "/lol/summoner/v4/summoners/by-name/fake") {
        return R"(
            {
                "id": "fake_id",
                "accountId": "6mRGRmn4lVmvdlLAHUbtL1gCnEHtaI9Hok70-dYu2VjszO4",
                "puuid": "LHOj588fg-vaVT04y8JJtOPuAU_5SWZY2fK_zHOQzNgopdgj_xMTqT2g7ebTseuLJB0VFuMFVBOzGg",
                "name": "fake",
                "profileIconId": 3553,
                "revisionDate": 1608568765000,
                "summonerLevel": 207
            }
        )";
    }
    else if (url == "/lol/spectator/v4/active-games/by-summoner/fake_id") {
        return R"(
            {
                "gameId": 4993850331,
                "mapId": 11,
                "gameMode": "CLASSIC",
                "gameType": "MATCHED_GAME",
                "gameQueueConfigId": 420,
                "participants": [
                    {
                        "teamId": 100,
                        "spell1Id": 11,
                        "spell2Id": 4,
                        "championId": 245,
                        "profileIconId": 654,
                        "summonerName": "Lyncas1",
                        "bot": false,
                        "summonerId": "zEkRLL0Ct98mkFo1EaU7jIN6RieA7DVOH2wUtv3IBZL0bwI",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                8128,
                                8143,
                                8138,
                                8106,
                                8313,
                                8347,
                                5005,
                                5008,
                                5002
                            ],
                            "perkStyle": 8100,
                            "perkSubStyle": 8300
                        }
                    },
                    {
                        "teamId": 100,
                        "spell1Id": 4,
                        "spell2Id": 7,
                        "championId": 145,
                        "profileIconId": 693,
                        "summonerName": "Sedr1on",
                        "bot": false,
                        "summonerId": "nwiWQ0dMb-rO21dLgu6W8sLYanrgQgENHjdCR8108Gxeqjc",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                9923,
                                8139,
                                8138,
                                8135,
                                8009,
                                9103,
                                5005,
                                5008,
                                5002
                            ],
                            "perkStyle": 8100,
                            "perkSubStyle": 8000
                        }
                    },
                    {
                        "teamId": 100,
                        "spell1Id": 4,
                        "spell2Id": 14,
                        "championId": 236,
                        "profileIconId": 7,
                        "summonerName": "MRS xKenzuke",
                        "bot": false,
                        "summonerId": "NXTet7TWZ2u9fIKNAV2NcMqtBHj3CLyfROmMCiWdqkQHg6k",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                8005,
                                8009,
                                9103,
                                8014,
                                8345,
                                8304,
                                5005,
                                5008,
                                5003
                            ],
                            "perkStyle": 8000,
                            "perkSubStyle": 8300
                        }
                    },
                    {
                        "teamId": 100,
                        "spell1Id": 3,
                        "spell2Id": 4,
                        "championId": 267,
                        "profileIconId": 3553,
                        "summonerName": "fake",
                        "bot": false,
                        "summonerId": "6BvVObGAfiv3rtN1NaQKVt2myz230Id7IATvdaSrnmS3UYk",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                8214,
                                8226,
                                8210,
                                8237,
                                8345,
                                8347,
                                5008,
                                5003,
                                5002
                            ],
                            "perkStyle": 8200,
                            "perkSubStyle": 8300
                        }
                    },
                    {
                        "teamId": 100,
                        "spell1Id": 12,
                        "spell2Id": 4,
                        "championId": 68,
                        "profileIconId": 764,
                        "summonerName": "Odoamne",
                        "bot": false,
                        "summonerId": "YSbXgnASs8v5gz9puhnkrGsgMyGp_Ad7uq6iEKopHoQ5a20",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                8229,
                                8275,
                                8233,
                                8236,
                                8139,
                                8135,
                                5007,
                                5008,
                                5002
                            ],
                            "perkStyle": 8200,
                            "perkSubStyle": 8100
                        }
                    },
                    {
                        "teamId": 200,
                        "spell1Id": 12,
                        "spell2Id": 4,
                        "championId": 84,
                        "profileIconId": 4858,
                        "summonerName": "Yusa1",
                        "bot": false,
                        "summonerId": "iPrt_wZ_-60wLIBkQPtIbgmSbpDCW4dctXf_HIpZI2cxfzA",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                8021,
                                8009,
                                9105,
                                8014,
                                8444,
                                8451,
                                5005,
                                5008,
                                5002
                            ],
                            "perkStyle": 8000,
                            "perkSubStyle": 8400
                        }
                    },
                    {
                        "teamId": 200,
                        "spell1Id": 4,
                        "spell2Id": 14,
                        "championId": 12,
                        "profileIconId": 4834,
                        "summonerName": "5R Brolia",
                        "bot": false,
                        "summonerId": "jCq7xMJpSTRUhkfkutZTmHNqVywBVXQlezR5cBYXBSNadzbE",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                8230,
                                8275,
                                8234,
                                8232,
                                8306,
                                8321,
                                5007,
                                5003,
                                5002
                            ],
                            "perkStyle": 8200,
                            "perkSubStyle": 8300
                        }
                    },
                    {
                        "teamId": 200,
                        "spell1Id": 4,
                        "spell2Id": 12,
                        "championId": 62,
                        "profileIconId": 4834,
                        "summonerName": "Agadmator fan",
                        "bot": false,
                        "summonerId": "_uj0ZnuJ-0BEjw8LmtBkD8Ut6vF5JqzzU3UUF3p6lZ6Cc1uo",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                8010,
                                9111,
                                9104,
                                8299,
                                8444,
                                8451,
                                5005,
                                5008,
                                5003
                            ],
                            "perkStyle": 8000,
                            "perkSubStyle": 8400
                        }
                    },
                    {
                        "teamId": 200,
                        "spell1Id": 4,
                        "spell2Id": 7,
                        "championId": 360,
                        "profileIconId": 7,
                        "summonerName": "Mini Deft",
                        "bot": false,
                        "summonerId": "KfFXoryQpI31KfcxefSfbwTP5lV2yZq_yN_pUuTShYf5-UU",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                8010,
                                8009,
                                9103,
                                8014,
                                8139,
                                8135,
                                5005,
                                5008,
                                5002
                            ],
                            "perkStyle": 8000,
                            "perkSubStyle": 8100
                        }
                    },
                    {
                        "teamId": 200,
                        "spell1Id": 4,
                        "spell2Id": 11,
                        "championId": 121,
                        "profileIconId": 3553,
                        "summonerName": "Charlie Heaton",
                        "bot": false,
                        "summonerId": "NfSurEqoqzXajQrNiCvM7AVjrfB1fX8P6_u1zjksGoq8ciW4",
                        "gameCustomizationObjects": [],
                        "perks": {
                            "perkIds": [
                                8230,
                                8275,
                                8233,
                                8232,
                                8143,
                                8135,
                                5008,
                                5008,
                                5003
                            ],
                            "perkStyle": 8200,
                            "perkSubStyle": 8100
                        }
                    }
                ],
                "observers": {
                    "encryptionKey": "I5TwIdIBFfdyyRb1GoybL0ivIk8cqjkD"
                },
                "platformId": "EUW1",
                "bannedChampions": [
                    {
                        "championId": 518,
                        "teamId": 100,
                        "pickTurn": 1
                    },
                    {
                        "championId": 34,
                        "teamId": 100,
                        "pickTurn": 2
                    },
                    {
                        "championId": -1,
                        "teamId": 100,
                        "pickTurn": 3
                    },
                    {
                        "championId": 876,
                        "teamId": 100,
                        "pickTurn": 4
                    },
                    {
                        "championId": 427,
                        "teamId": 100,
                        "pickTurn": 5
                    },
                    {
                        "championId": 104,
                        "teamId": 200,
                        "pickTurn": 6
                    },
                    {
                        "championId": 141,
                        "teamId": 200,
                        "pickTurn": 7
                    },
                    {
                        "championId": 80,
                        "teamId": 200,
                        "pickTurn": 8
                    },
                    {
                        "championId": 89,
                        "teamId": 200,
                        "pickTurn": 9
                    },
                    {
                        "championId": 163,
                        "teamId": 200,
                        "pickTurn": 10
                    }
                ],
                "gameStartTime": 1608722436981,
                "gameLength": 559
            }
        )";
    }
    else {
        return {};
    }
}

}  // namespace algue::algued
