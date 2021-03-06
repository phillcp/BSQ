/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgueifao <mgueifao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/11 22:23:35 by mgueifao          #+#    #+#             */
/*   Updated: 2020/11/11 22:56:54 by mgueifao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	g_error;

/*
** Reads a line from fd up to '\n' and returns a string containing the content
** without the LF.
**
** Returns NULL if a read error occured or the memory couldn't be allocated.
*/

char	*read_line(int fd, int pos)
{
	char	c;
	int		i;
	char	*line;

	i = read(fd, &c, 1);
	if (i == -1)
		g_error = 3;
	if (i == -1)
		return (NULL);
	if (!i && pos == 0)
		return (NULL);
	if (!i || c == '\n' || c == '\r')
	{
		if (!(line = malloc(pos + 1)))
			g_error = 2;
		if (g_error != 0)
			return (NULL);
		c = '\0';
	}
	else if (!(line = read_line(fd, pos + 1)))
		return (NULL);
	line[pos] = c;
	return (line);
}

int		add_first_line(t_map *map, char *line)
{
	int j;

	j = -1;
	if (!(map->obs_count = malloc(sizeof(int) * map->height * str_len(line))))
	{
		free(map);
		free(line);
		g_error = 2;
		return (0);
	}
	map->width = str_len(line);
	while (line[++j])
	{
		if (line[j] != map->obstacle && line[j] != map->empty)
		{
			free(line);
			return (clear_map(map));
		}
		map->obs_count[j] = (line[j] == map->obstacle ? 1 : 0);
		if (j != 0)
			map->obs_count[j] += map->obs_count[j - 1];
		if (map->obs_count[j])
			map->valid = 1;
	}
	return (1);
}

int		add_n_line(t_map *map, char *line, int i, int j)
{
	if (str_len(line) != map->width)
	{
		free(line);
		return (clear_map(map));
	}
	while (line[++j])
	{
		if (line[j] != map->obstacle && line[j] != map->empty)
		{
			free(line);
			return (clear_map(map));
		}
		map->obs_count[j + map->width * i] = (line[j] == map->obstacle ? 1 : 0);
		if (j == 0)
			map->obs_count[j + map->width * i] +=\
				map->obs_count[j + map->width * (i - 1)];
		else
			map->obs_count[j + map->width * i] += map->obs_count[j + map->width\
				* (i - 1)] + map->obs_count[j + map->width * i - 1] -\
				map->obs_count[j + map->width * (i - 1) - 1];
		if (map->obs_count[j + map->width * i])
			map->valid = 1;
	}
	return (1);
}

int		add_line(t_map *map, char *line, int i)
{
	if (i == 0)
		return (add_first_line(map, line));
	else if (!(add_n_line(map, line, i, -1)))
		return (0);
	free(line);
	return (1);
}

t_map	*read_map(int fd)
{
	t_map	*map;
	int		i;
	char	*line;

	if (!(map = init(fd)))
		return (NULL);
	i = -1;
	while (++i < map->height && (line = read_line(fd, 0)))
		if (!add_line(map, line, i))
			return (NULL);
	if (i < map->height || !map->valid)
		g_error = 1;
	if (!map->valid)
		clear_map(map);
	if (g_error != 0)
		return (NULL);
	while ((line = read_line(fd, 0)))
		if (str_len(line) > 0)
		{
			g_error = 1;
			return (NULL);
		}
	if (g_error != 0)
		return (NULL);
	return (map);
}
