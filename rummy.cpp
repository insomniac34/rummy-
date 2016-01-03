#include <deque>
#include <algorithm>
#include <vector>
#include <iostream>
#include <map>
#include "time.h"

class Card {
private:

public:
	std::string suit;
	char val;

	int sortOrder;
	bool inPlay;

	std::string getCardName() {
		//std::string str = (std::string(&val).append("_").append(suit));
		//std::cout << "str is " << str << std::endl;
		std::string cardVal = std::string(&val);
		if (cardVal.compare("0") == 0) {
			cardVal = "10";
		}
		return cardVal.append("_").append(suit);
	}

	Card() {

	}

	~Card() {

	}

	Card(std::string suit, char val) {
		this->suit = suit;
		this->val = val;
	}
};

class Player {
private:

public:
	Player() {

	}

	~Player() {

	}

	std::deque<Card*> cards;
	std::string name;
	std::deque<Card*> cardsInPlay;

	Player(std::string name, std::deque<Card*> cards) {
		this->cards = cards;
		this->name = name;
	}
};

char vals[] = {'2', '3', '4', '5', '6', '7', '8', '9', '0', 'J', 'Q', 'K', 'A'};
std::string suits[] = {"Hearts", "Spades", "Diamonds", "Clubs"};

int vals_indexOf(char c) {
	int i = 0;
	for (char val : vals) {
		if (val == c) {
			return i;
		}
		i++;
	} 
}

bool sortByOrder (const Card *cardA, const Card *cardB) {
	return cardA->sortOrder < cardB->sortOrder;
}

std::deque<Card*> &sortCardsByVal(std::deque<Card*> &hand) {
	for (Card *card : hand) {
		card->sortOrder = vals_indexOf(card->val);
	}
	std::sort(hand.begin(), hand.end(), sortByOrder);
	return hand;
}

int main(int argc, char **argv) {
	std::deque<Card*> deck;

	for (std::string suit : suits) {
		for (char val : vals) {
			deck.push_back(new Card(suit, val));	
		}		
	}

	srand(time(0));
	std::random_shuffle(deck.begin(), deck.end());

	// for (Card *card : deck) {
	// 	std::cout << card->suit << ", " << card->val << std::endl;
	// }

	std::deque<Card*> discards;
	std::deque<Player*> players;

	std::deque<Card*> player1Cards;
	std::deque<Card*> player2Cards;

	for (auto i = 0; i < 7; i++) {
		player1Cards.push_front(deck.front());
		deck.pop_front();
	}

	for (auto i = 0; i < 7; i++) {
		player2Cards.push_front(deck.front());
		deck.pop_front();
	}	

	// for (Card *card : player1Cards) {
	// 	std::cout << card->suit << " " << card->val << std::endl;
	// }
	// std::cout << "length of main deck is " << deck.size() << std::endl;

	players.push_front(new Player("Player1", player1Cards));
	players.push_front(new Player("Player2", player2Cards));

	discards.push_front(deck.front());
	deck.pop_front();

	//begin game...
	bool GAME_ACTIVE = true;
	while(GAME_ACTIVE) {
		for (Player *player : players) {

			//check discard pile:
			//std::deque<Card*> playableCards;

			//generate mappings by suit
			std::map<std::string, std::deque<Card*> > suitToCardMappings;
			for (std::string suit : suits) {
				suitToCardMappings[suit] = std::deque<Card*>();
				for (Card *card : player->cards) {
					if (suit.compare(card->suit) == 0) {
						suitToCardMappings[suit].push_front(card);
					}
				}				
			}

			for (std::string suit : suits) {
				std::cout << "Cards in suit " << suit << std::endl;
				for (Card *card : suitToCardMappings[suit]) {
					std::cout << card->getCardName() << std::endl;
				}
				std::cout << std::endl;
			}

			//see if any of the suit mappings are at least 3 in size and in numeric order
			for (std::string suit : suits) {				
				if (suitToCardMappings[suit].size() >= 3) {
					//sort the same-suit cards					
					suitToCardMappings[suit] = sortCardsByVal(suitToCardMappings[suit]);
					std::cout << "order: " << std::endl;
					for (Card *card : suitToCardMappings[suit]) {
						std::cout << card->getCardName() << std::endl;
					}
					//check for a numerical run
					bool RUN_POSSIBLE = true;
					bool IS_RUN = false;
					int index = 0;
					int runCount = 1;
					while (RUN_POSSIBLE) {
						//if the current card's value + 1 is = to the next card's value, it's in numeric order
						printf("Is current card's val plus one %d equal to the next card's val %d?\n", vals_indexOf((suitToCardMappings[suit][index])->val)+1, vals_indexOf((suitToCardMappings[suit][index+1])->val));
						if (vals_indexOf((suitToCardMappings[suit][index])->val)+1 == vals_indexOf((suitToCardMappings[suit][index+1])->val)) {
							runCount++;
							printf("It is! Setting run count to %d\n", runCount);
							if (runCount == 3) {
								IS_RUN = true;
								printf("runCount is now at minimum for a run: setting IS_RUN to true!\n");
							}
						}
						else {
							printf("No!\n");
							//now, we need to see if the previous run was valid
							if (runCount == 2) {
								printf("But there was 2 cards in order found, so resetting runCount to 1!\n");
								runCount = 1;
							}
							else if (IS_RUN) {
								printf("But it IS the end of a run! Setting all cards at index %d through index %d to played!\n", index-(runCount-1), index);
								//if it is a run, then we need to set the cards in the run to be played
								for (auto i = (index - (runCount-1)); i <= index; i++) {
									(suitToCardMappings[suit][i])->inPlay = true;
								}
								printf("resetting run!\n");
								runCount = 1;
								IS_RUN = false;
							}
						}

						index++;
						if (index == suitToCardMappings[suit].size() - 1) {
							RUN_POSSIBLE = false;
							if (IS_RUN) {
								//if it is a run, then we need to set the cards in the run to be played
								for (auto i = (index - (runCount-1)); i <= index; i++) {
									(suitToCardMappings[suit][i])->inPlay = true;
								}								
							}
						}
					}	
					for (auto i = 0; i < suitToCardMappings[suit].size(); i++) {
						if ((suitToCardMappings[suit][i])->inPlay) {
							std::cout << (suitToCardMappings[suit][i])->getCardName() << " is in play!" << std::endl;	
						}							
					}
				}
			}

			//generate mappings for three of a kind 
			std::map<char, std::deque<Card*> > valToCardMappings;
			for (char val : vals) {
				valToCardMappings[val] = std::deque<Card*>();
				for (Card *card : player->cards) {
					if (val == card->val && card->inPlay == false) {
						valToCardMappings[val].push_front(card);
					}
				}				
			}

			//now see if there are any sets of 3 or 4
			for (char val : vals) {
				if (valToCardMappings[val].size() > 2) {
					for (auto i = 0; i < valToCardMappings[val].size(); i++) {
						(valToCardMappings[val][i])->inPlay = true;
					}
				}
			}

			for (Card *card : player->cards) {
				if (card->inPlay) {
					std::cout << "In play: " << card->getCardName() << std::endl;
				}
			}

			GAME_ACTIVE = false;

			// std::cout << "val mappings: " << std::endl;
			// for (char val : vals) {
			// 	std::cout << val << ": " << std::endl;
			// 	for (Card *card : valToCardMappings[val]) {
			// 		std::cout << card->suit << ", " << card->val << std::endl;
			// 	}
			// }
			// GAME_ACTIVE = false;


		}
	}

	return 0;
}





