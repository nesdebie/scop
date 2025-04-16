/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 10:51:19 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/16 11:00:15 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Model.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " /models/<name>.obj" << std::endl;
		return (1);
	}

	std::ifstream file(argv[1]);
	if (!file.is_open())
	{
		std::cerr << "Error: File not found or inaccessible: " << argv[1] << std::endl;
		return (1);
	}

	Model model;
	bool loaded = model.loadFromOBJ(argv[1]);
	std::cout << "Model loaded: " << (loaded ? "true" : "false") << std::endl;
	return (0);
}