select count(person_id) from people where born = ( select premiered from titles where primary_title = 'The Prestige' and premiered is not null);
