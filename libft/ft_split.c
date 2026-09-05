/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 14:25:28 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:10:52 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	count_words(char const *s, char c)
{
	size_t	words;
	size_t	i;

	words = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] != c && (s[i + 1] == c || s[i + 1] == '\0'))
			words++;
		i++;
	}
	return (words);
}

static void	copy_word(char *nw, char const *s, char c)
{
	size_t	i;

	i = 0;
	while (s[i] && s[i] != c)
	{
		nw[i] = s[i];
		i++;
	}
	nw[i] = '\0';
}

static void	free_words(char **dest, size_t w)
{
	size_t	i;

	i = 0;
	while (i < w)
	{
		free(dest[i]);
		i++;
	}
	free(dest);
}

static int	malloc_words(char **dest, char const *s, char c)
{
	size_t	j;
	size_t	i;
	size_t	w;

	i = 0;
	w = 0;
	while (s[i])
	{
		j = 0;
		while (s[i + j] && s[i + j] != c)
			j++;
		if (j > 0)
		{
			dest[w] = malloc(sizeof(char) * (j + 1));
			if (!dest[w])
				return (free_words(dest, w), 0);
			copy_word(dest[w], (s + i), c);
			w++;
			i = i + j;
		}
		else
			i++;
	}
	dest[w] = 0;
	return (1);
}

char	**ft_split(char const *s, char c)
{
	char	**dest;

	if (!s)
		return (NULL);
	dest = malloc(sizeof(char *) * (count_words(s, c) + 1));
	if (!dest)
		return (NULL);
	if (!malloc_words(dest, s, c))
		return (NULL);
	return (dest);
}
