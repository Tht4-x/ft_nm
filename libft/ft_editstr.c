/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_editstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 19:39:31 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:12:11 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_editstr(char *new, char *old)
{
	char	*dest;

	dest = ft_realloc(old, ft_strlen(new) + 1);
	if (!dest)
		return (NULL);
	ft_strcpy(dest, new);
	return (dest);
}
