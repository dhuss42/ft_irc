/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 14:12:34 by dhuss             #+#    #+#             */
/*   Updated: 2025/07/07 16:02:57 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_HPP
# define ERRORS_HPP

#include "ft_irc.hpp"

enum class ErrorCode
{
	E_ARGNBR,
	E_PRT,
	E_PRTNBR,
	E_PSSWRD,
	E_SCKFD,
	E_FCNTL,
	E_SCKOPT,
	E_BND,
	E_LSTN,
	E_PLL,
	E_ACCPT,
	E_SCKEMPTY,
	E_RCV,
	E_SND,
};

class Errors : public std::runtime_error
{
	private:
		ErrorCode	_code;
		static std::string make_message(ErrorCode code);
	public:
		Errors(ErrorCode code);
		~Errors();

		static void	handleErrors(const std::exception& e);
		// ErrorCode getErrorCode() const noexcept;
};

#endif