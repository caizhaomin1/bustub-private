select primary_title, COALESCE(ended, strftime('%Y', 'now')) - premiered as y from titles where type='tvSeries' and premiered != 'NULL' order by y desc , primary_title desc limit 20;
