# Generated by Django 3.0.2 on 2020-09-20 18:50

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('database', '0011_contest_registertimeup'),
    ]

    operations = [
        migrations.AlterField(
            model_name='contest',
            name='registerTimeUp',
            field=models.DateTimeField(),
        ),
    ]